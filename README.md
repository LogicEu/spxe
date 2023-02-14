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
## header only

> As a header only solution, you need to define 
> SPXE_APPLICATION before including spxe.h to access the
> implementation details. 

```C
#define SPXE_APPLICATION
#include <spxe.h>
```

> You should only define SPXE_APPLICATION in a single
> translation unit.

## dependencies

* [GLFW](https://github.com/glfw/glfw)
* [GLEW](https://github.com/nigels-com/glew) (only on Linux and Windows)

## OS compilation flags

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
