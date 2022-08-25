#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SPXE_APPLICATION
#include <spxe.h>

#define pxat(px, width, x, y) (px[(((y) * (width)) + (x)) * 4])

static const unsigned char red[] = {255, 0, 0, 255};
static const unsigned char sand[] = {125, 125, 0, 255};

static inline void pxAir(unsigned char* pixels, const int width, const int height, const int x, const int y)
{
    unsigned char air[] = {100, 100, 130 + (int)(125.0 * ((float)y / (float)height)), 255};
    memcpy(pixels + ((y * width) + x) * 4, air, 4);
}

static void pixelsInit(unsigned char* pixels, const int width, const int height)
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (y < ((height / 3) - (rand() % 40))) {
                memcpy(pixels + (y * width + x) * 4, sand, 4);
            }
            else pxAir(pixels, width, height, x, y);
        }
    }
}

static void pixelsUpdate(unsigned char* pixels, const int width, const int height)
{
    static unsigned char buf[0xffffff];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int index = (y * width + x) * 4;
            if (pixels[index] > 124) {
                memcpy(pixels + (y * width + x) * 4, sand, 4);
                if (y > 0) {
                    if (pxat(pixels, width, x, y - 1) < 125) {
                        memcpy(buf + ((y - 1) * width + x) * 4, sand, 4);
                        pxAir(buf, width, height, x, y);
                    }
                    else if (x + 1 < width && pxat(pixels, width, x + 1, y - 1) < 125) {
                        memcpy(buf + ((y - 1) * width + x + 1) * 4, sand, 4);
                        pxAir(buf, width, height, x, y);
                    }
                    else if (x > 0 && pxat(pixels, width, x - 1, y - 1) < 125) {
                        memcpy(buf + ((y - 1) * width + x - 1) * 4, sand, 4);
                        pxAir(buf, width, height, x, y);
                    }
                }
            }
            else pxAir(buf, width, height, x, y);
        }
    }

    memcpy(pixels, buf, width * height * 4);
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
  
    unsigned char* pixels = spxeStart("Pixel Sand Simulation", 800, 600, width, height);

    srand(time(NULL));
    pixelsInit(pixels, width, height);

    while (spxeRun(pixels)) {
        
        if (spxeKeyPressed(ESCAPE) || spxeKeyPressed(Q)) {
            break;
        }
        
        if (spxeKeyPressed(R)) {
            pixelsInit(pixels, width, height);
        }
        
        spxeMousePos(&mousex, &mousey);
        pixelsUpdate(pixels, width, height);
        if (spxeMouseDown(LEFT) && mousex >= 0 && mousex < width && mousey >= 0 && mousey < height) {
            memcpy(&pxat(pixels, width, mousex, mousey), red, 4);
        }
    }
    
    return spxeEnd(pixels);
}
