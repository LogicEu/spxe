#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SPXE_APPLICATION
#include <spxe.h>

#define pxat(px, width, x, y) (px[(((y) * (width)) + (x))].r)

static const Px red = {255, 0, 0, 255};
static const Px sand = {125, 125, 0, 255};

static inline void pxAir(Px* pixbuf, const int width, const int height, const int x, const int y)
{
    Px air = {100, 100, 130 + (int)(125.0 * ((float)y / (float)height)), 255};
    memcpy(pixbuf + ((y * width) + x), &air, sizeof(Px));
}

static void pixelsInit(Px* pixbuf, const int width, const int height)
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (y < ((height / 3) - (rand() % 40))) {
                memcpy(pixbuf + (y * width + x), &sand, sizeof(Px));
            }
            else pxAir(pixbuf, width, height, x, y);
        }
    }
}

static void pixelsUpdate(Px* pixbuf, const int width, const int height)
{
    static Px buf[0xfffff];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int index = (y * width + x);
            if (pixbuf[index].r > 124) {
                memcpy(pixbuf + (y * width + x), &sand, sizeof(Px));
                if (y > 0) {
                    if (pxat(pixbuf, width, x, y - 1) < 125) {
                        memcpy(buf + ((y - 1) * width + x), &sand, sizeof(Px));
                        pxAir(buf, width, height, x, y);
                    }
                    else if (x + 1 < width && pxat(pixbuf, width, x + 1, y - 1) < 125) {
                        memcpy(buf + ((y - 1) * width + x + 1), &sand, sizeof(Px));
                        pxAir(buf, width, height, x, y);
                    }
                    else if (x > 0 && pxat(pixbuf, width, x - 1, y - 1) < 125) {
                        memcpy(buf + ((y - 1) * width + x - 1), &sand, sizeof(Px));
                        pxAir(buf, width, height, x, y);
                    }
                }
            }
            else pxAir(buf, width, height, x, y);
        }
    }

    memcpy(pixbuf, buf, width * height * sizeof(Px));
}

int main(const int argc, char** argv)
{
    int mousex, mousey;
    int width = 160, height = 120;
    
    if (argc > 1) {
        width = height = atoi(argv[1]);
    }
    if (argc > 2) {
        height = atoi(argv[2]);
    }
  
    Px* pixbuf = spxeStart("Pixel Sand Simulation", 800, 600, width, height);

    srand(time(NULL));
    pixelsInit(pixbuf, width, height);

    while (spxeRun(pixbuf)) {
        
        if (spxeKeyPressed(ESCAPE) || spxeKeyPressed(Q)) {
            break;
        }
        
        if (spxeKeyPressed(R)) {
            pixelsInit(pixbuf, width, height);
        }
        
        spxeMousePos(&mousex, &mousey);
        pixelsUpdate(pixbuf, width, height);
        if (spxeMouseDown(LEFT) && mousex >= 0 && mousex < width && mousey >= 0 && mousey < height) {
            memcpy(&pxat(pixbuf, width, mousex, mousey), &red, sizeof(Px));
        }
    }
    
    return spxeEnd(pixbuf);
}
