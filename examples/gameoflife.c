#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SPXE_APPLICATION
#include <spxe.h>

#define pxat(px, width, x, y) (px[(((y) * (width)) + (x)) * 4])

static unsigned char black[4] = {0, 0, 0, 255};
static unsigned char white[4] = {255, 255, 255, 255};
static unsigned char red[4] = {255, 0, 0, 255};

static void pixelsInitRand(unsigned char* pixels, const size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        memcpy(pixels + i * 4, rand() % 2 ? white : black, 4);
    }
}

static void pixelsUpdate(unsigned char* pixels, const int width, const int height)
{
    static int timers[3] = {1, 2, 3};
    static unsigned char buf[0xffffff];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
        
            int count = 0;
            count += (x + 1 < width) && pxat(pixels, width, x + 1, y);
            count += (x + 1 < width) && (y > 0) && pxat(pixels, width, x + 1, y - 1);
            count += (y > 0) && pxat(pixels, width, x, y - 1);
            count += (x > 0) && (y > 0) && pxat(pixels, width, x - 1, y - 1);
            count += (x > 0) && pxat(pixels, width, x - 1, y);
            count += (x > 0) && (y + 1 < height) && pxat(pixels, width, x - 1, y + 1);
            count += (y + 1 < height) && pxat(pixels, width, x, y + 1);
            count += (x + 1 < width) && (y + 1 < height) && pxat(pixels, width, x + 1, y + 1);

            const int index = (y * width + x) * 4;
            if (pixels[index]) {
                memcpy(buf + index, (count == 2 || count == 3) ? white : black, 4);
            }
            else memcpy(buf + index, (count == 3) ? white : black, 4);
        }
    }

    memcpy(pixels, buf, width * height * 4);
    
    // Color Update
    for (int i = 0; i < 3; ++i) {
        if (white[i] - timers[i] < 120 || white[i] - timers[i] > 255) {
            timers[i] = -timers[i];
        }
        white[i] -= timers[i];
    }

    black[1] += timers[0];
    black[2] += timers[1];
}

int main(const int argc, char** argv)
{
    int mousex, mousey;
    int width = 320, height = 240;
    
    if (argc > 1) {
        width = height = atoi(argv[1]);
    }
    if (argc > 2) {
        height = atoi(argv[2]);
    }
  
    unsigned char* pixels = spxeStart("Conway's Game of Life", 800, 600, width, height);

    srand(time(NULL));
    pixelsInitRand(pixels, width * height);

    while (spxeRun(pixels)) {
        
        if (spxeKeyPressed(ESCAPE) || spxeKeyPressed(Q)) {
            break;
        }
        
        if (spxeKeyPressed(R)) {
            pixelsInitRand(pixels, width * height);
        }
        
        spxeMousePos(&mousex, &mousey);

        pixelsUpdate(pixels, width, height);
        if (mousex >= 0 && mousex < width && mousey >= 0 && mousey < height) {
            memcpy(&pxat(pixels, width, mousex, mousey), red, 4);
        }
    }
    
    return spxeEnd(pixels);
}
