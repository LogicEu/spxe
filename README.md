# spxe

Simple pixel engine written in C using OpenGL. It's meant
to be extremely simple, lightweight, and easy to use. It
is compatible from C89 to C++20.

## Hello World
```C

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

```

## Plotting

By initializing spxe you are given a 8-bit RGBA pixel buffer in a row-major order 
where the coordinate (0, 0) corresponds to the top left pixel of the image. At each
frame you render from the CPU side by directly accessing and modifying the values of 
the pixel buffer giving you fast and direct control over every pixel in the screen. 
This is an example on how you can plot a pixel at position (x, y) to red.

```C

Px red = {255, 0, 0, 255};
pixbuf[y * width + x] = red;

```

Where ```width``` is the fourth argument given to ```spxeStart()```. 

## Header-Only

As a header only solution, you need to define 
SPXE_APPLICATION before including spxe.h to access the
implementation details. 

```C
#define SPXE_APPLICATION
#include <spxe.h>
```

You should only define SPXE_APPLICATION in a single
translation unit.

## Dependencies

* [GLFW](https://github.com/glfw/glfw)
* [GLEW](https://github.com/nigels-com/glew) (only on Linux and Windows)

## Compilation

These examples show a basic compilation command. You may need to use some -I and
-L flags depending on how you included your files and where in your system you 
keep the linking libraries.

* MacOS
```shell
clang source.c -o program -framework OpenGL -lglfw
```

* Linux
```shell
gcc source.c -o program -lGL -lGLEW -lglfw
```

* Windows
```shell
cl source.c -o program.exe -lopengl32 -lglfw3dll -lglew32
```

## Build Scripts

You can find two build scripts in this repository, one uses make and the other
one bash. They do essentially the same; provide a basic compilation command
giving a fast way to compile compile the examples and an easy install command to
make spxe available in your system. Both scripts very similarly and provide 
the same options. These are some of the things you can do with them:

Compile spxe as a shared or static library, using the 'shared' or 'static'
command line options. A library named libspxe.* will be created at the root of
the repository. The file extension will depend on the system.

```shell
make shared
./build.sh static
```

Compile a specific example by including spxe.h as a header only solution:

```shell
make example/mandelbrot.c
./build.sh example/mandelbrot.c
```

The resulting executable will be called a.out by default. To delete the
compiled binaries do:

```shell
make clean
./build.sh clean
```

Install spxe.h header file in /usr/local/include:

```shell
sudo make install
sudo ./build.sh install
```

Uninstall spxe.h header file from /usr/local/include:

```shell
sudo make uninstall
sudo ./build.sh uninstall
```

Print usage information and available commands:

```shell
./build.sh help
```

## API

```C
typedef struct Px {
    unsigned char r, g, b, a;
} Px;
```
Main and only tyepedefed structure used by the engine. The simple pixel 
engine handles only 4 channel RGBA buffers. You are encouraged to cast this
type to any other 32 bit pixel representation. 

```C
Px* spxeStart(
    const char* title, int winwidth, int winheight, int scrwidth, int scrheight
);
```
Creates a window with title, width and height specified by the first three
arguments. The fourth and fifth arguments represent the width and height of 
the pixel buffer that is allocated and returned to be rendered. The pixel 
buffer is a contiguous array of type Px in row-major order.

```C
int spxeRun(const Px* pixbuf);
```
Flushes previous frame from the screen and renders the pixel buffer passed as
argument on the opened window. If the window was closed this function returns
zero, otherwise, it returns a non-zero value.

```C
int spxeEnd(Px* pixbuf);
```
Deallocates the pixel buffer and closes the render window along with the glfw
library.

```C
void spxeScreenSize(int* width, int* height);
void spxeWindowSize(int* width, int* height);
```
These functions return the width and height of the initialized screen and
window respectively, inside the first and second pointers passed to the
function. Expects valid integers as arguments.

```C
int spxeKeyDown(const int key);
int spxeKeyPressed(const int key);
int spxeKeyReleased(const int key);
int spxeMouseDown(const int button);
int spxeMousePressed(const int button);
int spxeMouseReleased(const int button);
```
These keyboard and mouse input functions return 0 or 1 depending on the state 
of the key or button passed as argument. In the translation unit where spxe 
is being defined with the SPXE_APPLICATION macro, you can use shortcuts 
like ESCAPE, SPACE, SHIFT, LEFT_CONTROL, or LEFT and RIGHT for the mouse and
keyboard. Even just single capitalized characters work as character keys.
If you are calling these functions from elsewhere you need to pass in
[GLFW](https://github.com/glfw/glfw) key codes by including glfw3.h.

```C
char spxeKeyChar(void);
```
Returns the last character key pressed by the user.

```C
void spxeMousePos(int* xptr, int* yptr);
```
Gets the X and Y position of the mouse cursor relative to the pixel buffer
in screen coordinates and puts those values in the pointers passed in as
arguments. It will return negative or larger than the screen 
numbers if the mouse is outside of the screen. Returns valid pixel positions 
if the mouse is inside the pixel buffer and the render window. Expects valid
integers as arguments.

```C
void spxeMouseVisible(int);
```
Sets the native mouse cursor from your OS to be invisible if you pass a 0, or 
visible if you pass any other value.

```C
double spxeTime(void);
```
Returns time from the moment spxe was initialized in seconds.

### Example

This program opens up a 800 x 600 window in a 10:1 pixel scale.
Each frame it clears the pixel buffer to white, then plots the
center pixel red and the pixel pointed by the mouse blue.

```C

#define SPXE_APPLICATION
#include <spxe.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600
#define SCALE 1 / 10

int main(void)
{
    int mouseX, mouseY;
    const int width = WIDTH * SCALE, height = HEIGHT * SCALE;
    const int halfWidth = width / 2, halfHeight = height / 2;
    const size_t buflen = width * height * sizeof(Px);
    const Px red = {255, 0, 0, 255}, blue = {0, 0, 255, 0};
    
    Px* pixbuf = spxeStart("example", WIDTH, HEIGHT, width, height);
    if (!pixbuf) {
        fprintf(stderr, "could not init spxe\n");
        return 1;
    }

    while (spxeRun(pixbuf)) {
        spxeMousePos(&mouseX, &mouseY);
        if (spxeKeyPressed(ESCAPE)) {
            break;
        }

        memset(pixbuf, 255, buflen);
        pixbuf[halfHeight * width + halfWidth] = red;
        if (mouseX >= 0 && mouseX < width && mouseY >= 0 && mouseY < height) {
            pixbuf[mouseY * width + mouseX] = blue;
        }
    }

    return spxeEnd(pixbuf);
}

```

You can access pixel channels independently and easily. The examples folder in this repo
shows various ways of plotting and drawing with the engine.
