#ifndef SIMPLE_PIXEL_ENGINE_H
#define SIMPLE_PIXEL_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

/******************
****** spxe *******
Simple PiXel Engine
*******************
@Eugenio Arteaga A.
*******************

Simple pixel engine written in C and OpenGL using
GLFW. Meant to be extremely simple, lightweight,
and easy to use. By initializing spxe you instantly 
get an opened a window with a ready to use pixel 
perfect render context, handing the user control 
over every pixel that'll be rendered on the screen.

As a header only solution, you need to define 
SPXE_APPLICATION before including spxe.h to actually
define the implementation of the engine.

To use spxe.h across multiple compilation units you 
can include spxe.h in as many files as you need, but 
you should only define SPXE_APPLICATION in one. It's 
possible to create spxe.c or spxe.cpp file with only
these two lines:

#define SPXE_APPLICATION
#include <spxe.h>

The only external dependency on MacOS is GLFW. 
On Windows and Linux you also need GLEW.

To compile use the following flags:

MacOS:      -framework OpenGL -lglfw
Linux:      -lGL -lGLEW -lglfw
Windows:    -lopengl32 -lglfw3dll -lglew32

*************** Hello World Example ****************

#define SPXE_APPLICATION
#include <spxe.h>

int main(void)
{
    const int windowWidth = 800, windowHeight = 600;
    const int screenWidth = 200, screenHeight = 150;
    
    Px* pixbuffer = spxeStart(
        "Hello World", 
        windowWidth, 
        windowHeight, 
        screenWidth, 
        screenHeight
    );
    
    while (spxeRun(pixbuffer)) {
        if (spxeKeyPressed(ESCAPE)) {
            break;
        }
    }
    
    return spxeEnd(pixbuffer);
}

****************************************************/

/*  Simple PiXel Engine  */

typedef struct Px {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Px;

/* spxe core */
Px*     spxeStart(          const char* title,
                            const int   winwidth,   const int   winheight, 
                            const int   scrwidth,   const int   scrheight   );
int     spxeRun(            const Px*   pixbuffer                           );
int     spxeEnd(            Px*         pixbuffer                           );
Px*     spxePxGet(          const Px*   pixbuffer,  
                            const int   x,          const int y             );
void    spxePxSet(          Px*         dst,        const Px src            );

/* time input */
double  spxeTime(           void                                            );

/* keyboard input */
int     spxeKeyDown(        const int   key                                 );
int     spxeKeyPressed(     const int   key                                 );
int     spxeKeyReleased(    const int   key                                 );
char    spxeKeyChar(        void                                            );

/* mouse input */
void    spxeMousePos(       int*        x,          int*        y           );
int     spxeMouseDown(      const int   button                              );        
int     spxeMousePressed(   const int   button                              );
int     spxeMouseReleased(  const int   button                              );
void    spxeMouseVisible(   const int   visible                             );

#ifdef __cplusplus
}
#endif

#ifdef SPXE_APPLICATION

/******************
****** spxe *******
Simple PiXel Engine
*******************
@Eugenio Arteaga A.
*******************
* IMPLEMENTATION  *
******************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef __APPLE__
    #include <GL/glew.h>
    #define GLFW_MOD_CAPS_LOCK 0x0010
#else
    #define GL_SILENCE_DEPRECATION
    #define GLFW_INCLUDE_GLCOREARB
#endif

#include <GLFW/glfw3.h>

/* spxe shader strings */

#ifndef SPXE_SHADER_HEADER
    #ifdef __APPLE__ 
        #define SPXE_SHADER_HEADER "#version 330 core"
    #else 
        #define SPXE_SHADER_HEADER "#version 300 es\nprecision mediump float;\n\n"
    #endif
#endif

#define SPXE_SHADER_VERTEX "                    \
                                                \
layout (location = 0) in vec4 vertex;           \
out vec2 TexCoords;                             \
                                                \
void main(void)                                 \
{                                               \
    TexCoords = vertex.zw;                      \
    gl_Position = vec4(vertex.xy, 0.0, 1.0);    \
}"

#define SPXE_SHADER_FRAGMENT "                  \
                                                \
in vec2 TexCoords;                              \
out vec4 FragColor;                             \
uniform sampler2D tex;                          \
                                                \
void main(void)                                 \
{                                               \
    FragColor = texture(tex, TexCoords);        \
}"

static const char* vertexShader = SPXE_SHADER_HEADER SPXE_SHADER_VERTEX;
static const char* fragmentShader = SPXE_SHADER_HEADER SPXE_SHADER_FRAGMENT;

/* spxe core handler */

static struct spxeInfo {
    GLFWwindow* window;
    struct spxeRes {
        int width;
        int height;
    } scrres, winres;
    struct spxeRatio {
        float width;
        float height;
    } ratio;
    struct spxeInput {
        int mouseState;
        int queuedChar;
        int memChar;
        int keys[1024];
        int pressedKeys[1024];
    } input;
} spxe = {
    NULL, 
    {400, 300}, 
    {800, 600},
    {1.0, 1.0},
    {GLFW_RELEASE, 1, 0, {0}, {0}} 
};

/* implementation only static functions */

static void spxeFrame(void)
{
    const float w = (float)spxe.winres.width / (float)spxe.scrres.width;
    const float h = (float)spxe.winres.height / (float)spxe.scrres.height;
    
    spxe.ratio.width = (h < w) ? (h / w) : 1.0f;
    spxe.ratio.height = (w < h) ? (w / h) : 1.0f;

    const float vertices[] = {
        spxe.ratio.width,    spxe.ratio.height,   1.0f,   1.0f,
        spxe.ratio.width,    -spxe.ratio.height,  1.0f,   0.0f,
        -spxe.ratio.width,   -spxe.ratio.height,  0.0f,   0.0f,
        -spxe.ratio.width,   spxe.ratio.height,   0.0f,   1.0f
    };
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

static void spxeKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key < 128 && !spxe.input.keys[key]) {
        if (mods == GLFW_MOD_CAPS_LOCK || mods == GLFW_MOD_SHIFT || key < 65) {
            spxe.input.memChar = key;
        }
        else spxe.input.memChar = key + 32;
        spxe.input.queuedChar = 1;
    }

    spxe.input.keys[key] = action;
    spxe.input.pressedKeys[key] = spxe.input.pressedKeys[key] * (action != 0);
}

static void spxeWindow(GLFWwindow* window, int width, int height)
{
    spxe.winres.width = width;
    spxe.winres.height = height;
    spxeFrame();
}

/* time input */

double spxeTime(void)
{
    return glfwGetTime();
}

/* keyboard input */

int spxeKeyDown(const int key)
{
    return !!spxe.input.keys[key];
}

int spxeKeyPressed(const int key)
{
    if (!spxe.input.pressedKeys[key] && spxe.input.keys[key]) {
        spxe.input.pressedKeys[key] = 1;
        return 1;
    }
    return 0;
}

int spxeKeyReleased(const int key)
{
    if (!spxe.input.keys[key] && spxe.input.pressedKeys[key]) {
        spxe.input.pressedKeys[key] = 0;
        return 1;
    }
    return 0;
}

char spxeKeyChar(void)
{
    char ch = 0;
    if (spxe.input.queuedChar) {
        ch = spxe.input.memChar;
        spxe.input.queuedChar = 0;
    }
    return ch;
}

/* mouse input */

void spxeMousePos(int* x, int* y)
{
    double dx, dy;
    glfwGetCursorPos(spxe.window, &dx, &dy);

    const float width = (float)spxe.scrres.width, height = (float)spxe.scrres.height;
    const float hwidth = width * 0.5, hheight = height * 0.5;

    dx = dx * (width / (float)spxe.winres.width);
    dy = height - dy * (height / (float)spxe.winres.height);
    *x = (int)((dx - hwidth) / spxe.ratio.width + hwidth);
    *y = (int)((dy - hheight) / spxe.ratio.height + hheight);
}

int spxeMouseDown(const int button)
{
    return glfwGetMouseButton(spxe.window, button);
}

int spxeMousePressed(const int button)
{
    const int mouseButton = glfwGetMouseButton(spxe.window, button);
    const int pressed = (mouseButton == GLFW_PRESS) && 
                        (spxe.input.mouseState == GLFW_RELEASE);
    spxe.input.mouseState = mouseButton;
    return pressed;
}

int spxeMouseReleased(const int button)
{
    return !glfwGetMouseButton(spxe.window, button);
}

void spxeMouseVisible(const int visible)
{
    glfwSetInputMode(
        spxe.window, GLFW_CURSOR, 
        !visible ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
    );
}

/* spxe core */

Px* spxeStart(          const char* title, 
    const int winwidth, const int winheight, 
    const int scrwidth, const int scrheight)
{
    /* init glfw */
    if (!glfwInit()) {
        printf("spxe failed to initiate glfw.\n");
        return NULL;
    }

    /* open and setup window */
#ifdef __APPLE__
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(winwidth, winheight, title, NULL, NULL);
    if (!window) {
        printf("spxe failed to open glfw window.\n");
        glfwTerminate();
        return NULL;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetWindowSizeLimits(window, scrwidth, scrheight, GLFW_DONT_CARE, GLFW_DONT_CARE);
    
    glfwSetWindowSizeCallback(window, spxeWindow);
    glfwSetKeyCallback(window, spxeKeyboard);
    glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

    /* OpenGL context and settings */
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("spxe failed to initiate glew.\n");
        return NULL;
    }
#endif

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
    
    /* allocate pixel framebuffer */
    const size_t len = scrwidth * scrheight * sizeof(Px);
    Px* pixbuffer = malloc(len);
    if (!pixbuffer) {
        printf("spxe failed to allocate pixel framebuffer.\n");
        return NULL;
    }

    memset(pixbuffer, 155, len);

    /* set global information */
    spxe.window = window;
    spxe.winres.width = winwidth;
    spxe.winres.height = winheight;
    spxe.scrres.width = scrwidth;
    spxe.scrres.height = scrheight;
    
    /* compile and link shaders */
    unsigned int shader = glCreateProgram();
    
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShader, NULL);
    glCompileShader(vertex);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShader, NULL);
    glCompileShader(fragment);

    glAttachShader(shader, vertex);
    glAttachShader(shader, fragment);
    glLinkProgram(shader);
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    glUseProgram(shader);

    /* create shader buffers */
    const unsigned int indices[] = {
        0,  1,  3,
        1,  2,  3 
    };

    unsigned int id, vao, ebo;
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    glGenBuffers(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, vao);
    spxeFrame();
    
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vao);

    /* generate render texture */
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, spxe.scrres.width, spxe.scrres.height, 
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixbuffer
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    return pixbuffer;
}

int spxeRun(const Px* pixbuffer)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, spxe.scrres.width, spxe.scrres.height, 
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixbuffer
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(spxe.window);
    glfwPollEvents();
    
    return !glfwWindowShouldClose(spxe.window) && !!pixbuffer;
}

int spxeEnd(Px* pixbuffer)
{
    glfwDestroyWindow(spxe.window);
    glfwTerminate();
    
    if (!pixbuffer) {
        return EXIT_FAILURE;
    }
    
    free(pixbuffer);
    return EXIT_SUCCESS;
}

Px* spxePxGet(const Px* pixbuffer, const int x, const int y)
{
    return (Px*)(size_t)(pixbuffer + (y * spxe.scrres.width + x));
}

void spxePxSet(Px* dest, const Px src)
{
    memcpy(dest, &src, sizeof(Px));
}

#else

#include <GLFW/glfw3.h>

#endif // SPXE_APPLICATION

#define spxeKeyDown(key) spxeKeyDown(GLFW_KEY_ ## key)
#define spxeKeyPressed(key) spxeKeyPressed(GLFW_KEY_ ## key)
#define spxeKeyReleased(key) spxeKeyReleased(GLFW_KEY_ ## key)
#define spxeMouseDown(key) spxeMouseDown(GLFW_MOUSE_BUTTON_ ## key)
#define spxeMousePressed(key) spxeMousePressed(GLFW_MOUSE_BUTTON_ ## key)
#define spxeMouseReleased(key) spxeMouseReleased(GLFW_MOUSE_BUTTON_ ## key)

#endif // SIMPLE_PIXEL_ENGINE_H
