# spxe

> Simple pixel engine written in C using OpenGL. It's meant
> to be extremely simple, lightweight, and easy to use. By
> initializing spxe you instantly get a window with a ready
> to use pixel perfect render context, handing you the control
> over every pixel that is rendered on the screen.
>
> As a header only solution, you need to define 
> SPXE_APPLICATION before including spxe.h to access the
> implementation details. You should only define 
> SPXE_APPLICATION in a single translation unit.

```C
#define SPXE_APPLICATION
#include <spxe.h>
```

> The only external dependency on MacOS is GLFW. 
> On Windows and Linux you also need GLEW.

# OS compilation flags
## MacOS
```shell
-framework OpenGL -lglfw
```
## Linux
```shell
-lGL -lGLEW -lglfw
```
## Windows
```shell
-lopengl32 -lglfw3dll -lglew32
```

# Hello World Example
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