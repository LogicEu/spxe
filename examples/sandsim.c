#define SPXE_APPLICATION
#include <spxe.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const Px sand = {125, 125, 0, 255};

static Px pxAir(const int height, int y)
{
    Px air = {100, 100, 130, 255};
    air.b += (uint8_t)(int)(125.0 * ((float)y / (float)height));
    return air;
}

static void pxInit(Px* pixbuf, const int width, const int height)
{
    int x, y, index = 0;
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x, ++index) {
            pixbuf[index] = y < height / 3 - (rand() % 40) ? sand: pxAir(height, y);
        }
    }
}

static void pxUpdate(Px* pixbuf, Px* buf, const int width, const int height)
{
    int x, y, dy, index = 0;
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x, ++index) { 
            if (pixbuf[index].r > 124) {
                if (y > 0) {
                    dy = (y - 1) * width;
                    if (pixbuf[dy + x].r < 125) {
                        buf[dy + x] = sand;
                        buf[index] = pxAir(height, y);
                    } else if (x + 1 < width && pixbuf[dy + x + 1].r < 125) {
                        buf[dy + x + 1] = sand;
                        buf[index] = pxAir(height, y);
                    } else if (x > 0 && pixbuf[dy + x - 1].r < 125) {
                        buf[dy + x - 1] = sand;
                        buf[index] = pxAir(height, y);
                    }
                }
            }
            else buf[index] = pxAir(height, y);
        }
    }

    memcpy(pixbuf, buf, width * height * sizeof(Px));
}

int main(const int argc, const char** argv)
{
    Px* pixbuf, *buf;
    const Px red = {255, 0, 0, 255};
    int mousex, mousey, width = 160, height = 120;
    
    if (argc > 1) {
        width = atoi(argv[1]);
        height = argc > 2 ? atoi(argv[2]) : width;
    }

    srand(time(NULL));
    pixbuf = spxeStart("sandsim", 800, 600, width, height);
    buf = (Px*)malloc(width * height * sizeof(Px));
    pxInit(pixbuf, width, height);

    while (spxeRun(pixbuf)) {
        spxeMousePos(&mousex, &mousey);
        if (spxeKeyPressed(KEY_ESCAPE) || spxeKeyPressed(KEY_Q)) {
            break;
        }
        if (spxeKeyPressed(KEY_R)) {
            pxInit(pixbuf, width, height);
        }
        
        pxUpdate(pixbuf, buf, width, height);
        if (spxeMouseDown(MOUSE_LEFT) && 
            mousex >= 0 && mousex < width && mousey >= 0 && mousey < height) {
            pixbuf[mousey * width + mousex] = red;
        }
    }
    
    free(buf);
    return spxeEnd(pixbuf);
}

