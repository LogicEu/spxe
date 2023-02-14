#define SPXE_APPLICATION
#include <spxe.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define pxAt(px, w, x, y) (px[(((y) * (w)) + (x))].r)

static const Px sand = {125, 125, 0, 255};

static inline void pxAir(Px* pixbuf, int width, int height, int x, int y)
{
    Px air = {100, 100, 130 + (int)(125.0 * ((float)y / (float)height)), 255};
    memcpy(pixbuf + y * width + x, &air, sizeof(Px));
}

static void pxInit(Px* pixbuf, const int width, const int height)
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

static void pxUpdate(Px* pixbuf, Px* buf, const int width, const int height)
{
    int index = 0;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            
            if (pixbuf[index].r > 124) {
                memcpy(pixbuf + index, &sand, sizeof(Px)); 
                if (y > 0) {
                    if (pxAt(pixbuf, width, x, y - 1) < 125) {
                        memcpy(buf + ((y - 1) * width + x), &sand, sizeof(Px));
                        pxAir(buf, width, height, x, y);
                    }
                    else if (x + 1 < width && pxAt(pixbuf, width, x + 1, y - 1) < 125) {
                        memcpy(buf + ((y - 1) * width + x + 1), &sand, sizeof(Px));
                        pxAir(buf, width, height, x, y);
                    }
                    else if (x > 0 && pxAt(pixbuf, width, x - 1, y - 1) < 125) {
                        memcpy(buf + ((y - 1) * width + x - 1), &sand, sizeof(Px));
                        pxAir(buf, width, height, x, y);
                    }
                }
            }
            else pxAir(buf, width, height, x, y);

            ++index;
        }
    }

    memcpy(pixbuf, buf, width * height * sizeof(Px));
}

int main(const int argc, char** argv)
{
    Px* pixbuf, *buf;
    const Px red = {255, 0, 0, 255};
    int mousex, mousey, width = 160, height = 120;
    
    if (argc > 1) {
        width = height = atoi(argv[1]);
    }
    if (argc > 2) {
        height = atoi(argv[2]);
    }

    srand(time(NULL));
    pixbuf = spxeStart("sandsim", 800, 600, width, height);
    buf = (Px*)malloc(width * height * sizeof(Px));
    pxInit(pixbuf, width, height);

    while (spxeRun(pixbuf)) {
        spxeMousePos(&mousex, &mousey);
        if (spxeKeyPressed(ESCAPE) || spxeKeyPressed(Q)) {
            break;
        }
        if (spxeKeyPressed(R)) {
            pxInit(pixbuf, width, height);
        }
        
        pxUpdate(pixbuf, buf, width, height);
        if (spxeMouseDown(LEFT) && mousex >= 0 && mousex < width && mousey >= 0 && mousey < height) {
            memcpy(&pxAt(pixbuf, width, mousex, mousey), &red, sizeof(Px));
        }
    }
    
    free(buf);
    return spxeEnd(pixbuf);
}
