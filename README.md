# spxe

> Simple pixel engine written in C using OpenGL. It's meant
> to be extremely simple, lightweight, and easy to use. It
> is compatible from C89 to C++20.

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
## Header-Only

> As a header only solution, you need to define 
> SPXE_APPLICATION before including spxe.h to access the
> implementation details. 

```C
#define SPXE_APPLICATION
#include <spxe.h>
```

> You should only define SPXE_APPLICATION in a single
> translation unit.

## Dependencies

* [GLFW](https://github.com/glfw/glfw)
* [GLEW](https://github.com/nigels-com/glew) (only on Linux and Windows)

## Compilation

* MacOS
```shell
-framework OpenGL -lglfw
```

* Linux
```shell
-lGL -lGLEW -lglfw
```

* Windows
```shell
-lopengl32 -lglfw3dll -lglew32
```

## API

```C
typedef struct Px {
    unsigned char r, g, b, a;
};
```
> Main and only tyepedefed structure used by the engine. The simple pixel 
> engine handles only 4 channel RGBA buffers. You are encouraged to cast this
> type to any other 32 bit pixel representation. 

```C
Px* spxeStart(
    const char* title, int winwidth, int winheight, int scrwidth, int scrheight
);
```
> Creates a window with title, width and height specified by the first three
> arguments. The fourth and fifth arguments represent the width and height of 
> the pixel buffer that is allocated and returned to be rendered. The pixel 
> buffer is a contiguous array of type Px in row-major order.

```C
int spxeRun(const Px* pixbuf);
```
> Flushes previous frame from the screen and renders the pixel buffer passed as
> argument on the opened window. If the window was closed this function returns
> zero, otherwise, it returns a non-zero value.

```C
int spxeEnd(Px* pixbuf);
```
> Deallocates the pixel buffer and closes the render window along with the glfw
> library.

```C
void spxeScreenSize(int* width, int* height);
void spxeWindowSize(int* width, int* height);
```
> These functions return the width and height of the initialized screen and
> window respectively, inside the first and second pointers passed to the
> function. Expects valid integers as arguments.

```C
int spxeKeyDown(const int key);
int spxeKeyPressed(const int key);
int spxeKeyReleased(const int key);
int spxeMouseDown(const int button);
int spxeMousePressed(const int button);
int spxeMouseReleased(const int button);
```
> These keyboard and mouse input functions return 0 or 1 depending on the state 
> of the key or button passed as argument. In the translation unit where spxe 
> is being defined with the SPXE_APPLICATION macro, you can use shortcuts 
> like ESCAPE, SPACE, SHIFT, LEFT_CONTROL, or LEFT and RIGHT for the mouse and
> keyboard. Even just single capitalized characters work as character keys.
> If you are calling these functions from elsewhere you need to pass in
> [GLFW](https://github.com/glfw/glfw) key codes by including glfw3.h.

```C
char spxeKeyChar(void);
```
> Returns the last character key pressed by the user.

```C
void spxeMousePos(int* xptr, int* yptr);
```
> Gets the X and Y position of the mouse cursor relative to the pixel buffer
> in screen coordinates and puts those values in the pointers passed in as
> arguments. It will return negative or larger than the screen 
> numbers if the mouse is outside of the screen. Returns valid pixel positions 
> if the mouse is inside the pixel buffer and the render window. Expects valid
> integers as arguments.

```C
void spxeMouseVisible(int);
```
> Sets the native mouse cursor from your OS to be invisible if you pass a 0, or 
> visible if you pass any other value.

```C
double spxeTime(void);
```
> Returns time from the moment spxe was initialized in seconds.

### Access
```C

    /* Access pixel at position (x=2, y=1) and set it to be red /*
 
    const int width = 40;
    const int height = 30;

    int x = 2;
    int y = 1;

    Px* pixbuf = spxeStart("example", 800, 600, width, height);
    const Px red = {255, 0, 0, 255};
    pixbuf[ y * width + x] = red;

    /* Render with spxeRun(pixbuf); ... */

```
> You can access pixel channels independently easily. The examples in this repo
> show various ways of plotting and drawing with the engine.
