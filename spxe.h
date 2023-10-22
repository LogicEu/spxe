/*

Copyright (c) 2023 Eugenio Arteaga A.

Permission is hereby granted, free of charge, to any 
person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice 
shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef SIMPLE_PIXEL_ENGINE_H
#define SIMPLE_PIXEL_ENGINE_H

/******************
****** spxe *******
Simple PiXel Engine
*******************
@Eugenio Arteaga A.
*******************

Simple pixel engine written in C using OpenGL. It's meant
to be extremely simple, lightweight, and easy to use. By
initializing spxe you instantly get a window with a ready
to use pixel perfect render context, handing you the control
over every pixel that is rendered on the screen.

As a header only solution, you need to define 
SPXE_APPLICATION before including spxe.h to access the
implementation details. You should only define 
SPXE_APPLICATION in a single translation unit.

The only external dependency on MacOS is GLFW. 
On Windows and Linux you also need GLEW.

************** OS compliation flags ***************

MacOS:      -framework OpenGL -lglfw
Linux:      -lGL -lGLEW -lglfw
Windows:    -lopengl32 -lglfw3dll -lglew32

*************** Hello World Example ****************

#define SPXE_APPLICATION
#include <spxe.h>

int main(void)
{
    Px* pixbuf = spxeStart("spxe", 800, 600, 100, 75);
    while (spxeRun(pixbuf)) {
        if (spxeKeyPressed(ESCAPE)) {
            break;
        }
    }
    return spxeEnd(pixbuf);
}

****************************************************/

/*  Simple PiXel Engine  */

#ifndef PX_TYPE_DEFINED
#define PX_TYPE_DEFINED

typedef struct Px {
    unsigned char r, g, b, a;
} Px;

#endif /* PX_TYPE_DEFINED */

/* spxe core */
int     spxeStep(           void                                            );
int     spxeRun(            const Px*   pixbuf                              );
void    spxeRender(         const Px*   pixbuf                              );
int     spxeEnd(            Px*         pixbuf                              );
Px*     spxeStart(          const char* title,
                            const int   winwidth,   const int   winheight, 
                            const int   scrwidth,   const int   scrheight   );

void    spxeScreenSize(     int*        widthptr,   int*        heightptr   );
void    spxeWindowSize(     int*        widthptr,   int*        heightptr   );
void    spxeBackgroundColor(const Px    px                                  );

/* time input */
double  spxeTime(           void                                            );

/* keyboard input */
int     spxeKeyDown(        const int   key                                 );
int     spxeKeyPressed(     const int   key                                 );
int     spxeKeyReleased(    const int   key                                 );
char    spxeKeyChar(        void                                            );

/* mouse input */
void    spxeMousePos(       int*        xptr,       int*        yptr        );
int     spxeMouseDown(      const int   button                              );        
int     spxeMousePressed(   const int   button                              );
int     spxeMouseReleased(  const int   button                              );
void    spxeMouseVisible(   const int   visible                             );

#ifdef SPXE_APPLICATION

/******************
****** spxe *******
Simple PiXel Engine
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

#define _SPXE_TOK2STR(s) #s
#define SPXE_TOK2STR(s) _SPXE_TOK2STR(s)

#ifndef SPXE_SHADER_LAYOUT_LOCATION
    #define SPXE_SHADER_LAYOUT_LOCATION 0
#endif

#ifndef SPXE_SHADER_HEADER
    #ifdef __APPLE__ 
        #define SPXE_SHADER_HEADER "#version 330 core\n"
    #else 
        #define SPXE_SHADER_HEADER "#version 300 es\nprecision mediump float;\n"
    #endif
#endif

#define SPXE_SHADER_LAYOUT_STR SPXE_TOK2STR(SPXE_SHADER_LAYOUT_LOCATION)

#define SPXE_SHADER_VERTEX "layout (location = " SPXE_SHADER_LAYOUT_STR ")" \
"in vec4 vertCoord;\n"                          \
"out vec2 TexCoords;\n"                         \
"void main(void) {\n"                           \
"    TexCoords = vertCoord.zw;\n"               \
"    gl_Position = vec4(vertCoord.xy,0.,1.); \n"\
"}\n"

#define SPXE_SHADER_FRAGMENT                    \
"in vec2 TexCoords;\n"                          \
"out vec4 FragColor;\n"                         \
"uniform sampler2D tex;\n"                      \
"void main(void) {\n"                           \
"    FragColor = texture(tex, TexCoords);\n"    \
"}\n"

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
        unsigned char keys[1024];
        unsigned char pressedKeys[1024];
    } input;
} spxe = {NULL, {400, 300}, {800, 600}, {1.0, 1.0}, {GLFW_RELEASE, 1, 0, {0}, {0}}};

/* implementation only static functions */

static void spxeFrame(void)
{
    int i;

    float vertices[16] = {
        1.0f,   1.0f,   1.0f,   1.0f,
        1.0f,  -1.0f,   1.0f,   0.0f,
        -1.0f, -1.0f,   0.0f,   0.0f,
        -1.0f,  1.0f,   0.0f,   1.0f
    };

    const float w = (float)spxe.winres.width / (float)spxe.scrres.width;
    const float h = (float)spxe.winres.height / (float)spxe.scrres.height;
    
    spxe.ratio.width = (h < w) ? (h / w) : 1.0f;
    spxe.ratio.height = (w < h) ? (w / h) : 1.0f;

    for (i = 0; i < 16; i += 4) {
        vertices[i] *= spxe.ratio.width;
        vertices[i + 1] *= spxe.ratio.height;
    }
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

static void spxeKeyboard(GLFWwindow* win, int key, int code, int action, int mod)
{
    (void)win;
    (void)code;

    if (key < 128 && !spxe.input.keys[key]) {
        if (mod == GLFW_MOD_CAPS_LOCK || mod == GLFW_MOD_SHIFT || key < 65) {
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
    (void)window;
#ifndef __APPLE__
    glViewport(0, 0, width, height);
#endif /* __APPLE__ */
    spxe.winres.width = width;
    spxe.winres.height = height;
    spxeFrame();
}

/* window and screen size getters */

void spxeWindowSize(int* width, int* height)
{
    *width = spxe.winres.width;
    *height = spxe.winres.height;
}

void spxeScreenSize(int* width, int* height)
{
    *width = spxe.scrres.width;
    *height = spxe.scrres.height;
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
    float width, height, hwidth, hheight;
    
    glfwGetCursorPos(spxe.window, &dx, &dy);
    width = (float)spxe.scrres.width;
    height = (float)spxe.scrres.height;
    hwidth = width * 0.5;
    hheight = height * 0.5;

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

Px* spxeStart(          
    const char* title,  const int winwidth, const int winheight, 
    const int scrwidth, const int scrheight)
{
    Px* pixbuf;
    GLFWwindow* window;
    unsigned int id, vao, ebo, texture;
    unsigned int shader, vshader, fshader;

    const size_t scrsize = scrwidth * scrheight;
    const unsigned int indices[] = {
        0,  1,  3,
        1,  2,  3 
    };

    /* init glfw */
    if (!glfwInit()) {
        fprintf(stderr, "spxe failed to initiate glfw.\n");
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

    window = glfwCreateWindow(winwidth, winheight, title, NULL, NULL);
    if (!window) {
        fprintf(stderr, "spxe failed to open glfw window.\n");
        glfwTerminate();
        return NULL;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetWindowSizeLimits(window, scrwidth, scrheight, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowSizeCallback(window, spxeWindow);
    glfwSetKeyCallback(window, spxeKeyboard);
    glfwSetInputMode(window, GLFW_MOD_CAPS_LOCK, GLFW_TRUE);

    /* OpenGL context and settings */
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "spxe failed to initiate glew.\n");
        return NULL;
    }
#endif

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
    
    /* allocate pixel framebuffer */
    pixbuf = (Px*)calloc(scrsize, sizeof(Px));
    if (!pixbuf) {
        fprintf(stderr, "spxe failed to allocate pixel framebuffer.\n");
        return NULL;
    }

    /* set global information */
    spxe.window = window;
    spxe.winres.width = winwidth;
    spxe.winres.height = winheight;
    spxe.scrres.width = scrwidth;
    spxe.scrres.height = scrheight;
    
    /* compile and link shaders */
    shader = glCreateProgram();
    
    vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertexShader, NULL);
    glCompileShader(vshader);

    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fragmentShader, NULL);
    glCompileShader(fshader);

    glAttachShader(shader, vshader);
    glAttachShader(shader, fshader);
    glLinkProgram(shader);
    
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    
    glUseProgram(shader);

    /* create vertex buffers */
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    glGenBuffers(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, vao);
    spxeFrame();
    
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(SPXE_SHADER_LAYOUT_LOCATION);
    glVertexAttribPointer(SPXE_SHADER_LAYOUT_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vao);

    /* create render texture (framebuffer) */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, spxe.scrres.width, spxe.scrres.height, 
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixbuf
    );

    return pixbuf;
}

void spxeBackgroundColor(const Px c)
{
    const float n = 1.0F / 255.0F;
    glClearColor((float)c.r * n, (float)c.g * n, (float)c.b * n, (float)c.a * n);
}

void spxeRender(const Px* pixbuf)
{
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, spxe.scrres.width, spxe.scrres.height, 
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixbuf
    );

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int spxeStep(void)
{ 
    glfwPollEvents();
    glfwSwapBuffers(spxe.window);
    glClear(GL_COLOR_BUFFER_BIT);
    return !glfwWindowShouldClose(spxe.window);
}

int spxeRun(const Px* pixbuf)
{
    spxeRender(pixbuf);
    return spxeStep();
}

int spxeEnd(Px* pixbuf)
{
    glfwTerminate();
    if (pixbuf) {
        free(pixbuf);
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

#endif /* SPXE_APPLICATION */
#ifdef _glfw3_h_
#define spxeKeyDown(key) spxeKeyDown(GLFW_KEY_ ## key)
#define spxeKeyPressed(key) spxeKeyPressed(GLFW_KEY_ ## key)
#define spxeKeyReleased(key) spxeKeyReleased(GLFW_KEY_ ## key)
#define spxeMouseDown(key) spxeMouseDown(GLFW_MOUSE_BUTTON_ ## key)
#define spxeMousePressed(key) spxeMousePressed(GLFW_MOUSE_BUTTON_ ## key)
#define spxeMouseReleased(key) spxeMouseReleased(GLFW_MOUSE_BUTTON_ ## key)
#endif /* _glfw3_h_ */
#endif /* SIMPLE_PIXEL_ENGINE_H */
