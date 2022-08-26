#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SPXE_APPLICATION
#include <spxe.h>

#define pxat(px, width, x, y) (px[(((y) * (width)) + (x))].r)

static Px black = {0, 0, 0, 255};
static Px white = {255, 255, 255, 255};
static Px red = {255, 0, 0, 255};

static void pixelsInitRand(Px* pixbuf, const size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        memcpy(pixbuf + i, rand() % 2 ? &white : &black, sizeof(Px));
    }
}

static void pixelsUpdate(Px* pixbuf, const int width, const int height)
{
    static int timers[3] = {1, 2, 3};
    static Px buf[0xfffff];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
        
            int count = 0;
            count += (x + 1 < width) && pxat(pixbuf, width, x + 1, y);
            count += (x + 1 < width) && (y > 0) && pxat(pixbuf, width, x + 1, y - 1);
            count += (y > 0) && pxat(pixbuf, width, x, y - 1);
            count += (x > 0) && (y > 0) && pxat(pixbuf, width, x - 1, y - 1);
            count += (x > 0) && pxat(pixbuf, width, x - 1, y);
            count += (x > 0) && (y + 1 < height) && pxat(pixbuf, width, x - 1, y + 1);
            count += (y + 1 < height) && pxat(pixbuf, width, x, y + 1);
            count += (x + 1 < width) && (y + 1 < height) && pxat(pixbuf, width, x + 1, y + 1);

            const int index = (y * width + x);
            if (pixbuf[index].r) {
                memcpy(buf + index, (count == 2 || count == 3) ? &white : &black, sizeof(Px));
            }
            else memcpy(buf + index, (count == 3) ? &white : &black, sizeof(Px));
        }
    }

    memcpy(pixbuf, buf, width * height * sizeof(Px));
    
    // Color Update
    unsigned char* col = (unsigned char*)&white;
    for (int i = 0; i < 3; ++i) {
        if (col[i] - timers[i] < 120 || col[i] - timers[i] > 255) {
            timers[i] = -timers[i];
        }
        col[i] -= timers[i];
    }

    black.g += timers[0];
    black.b += timers[1];
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
  
    Px* pixbuf = spxeStart("Conway's Game of Life", 800, 600, width, height);

    srand(time(NULL));
    pixelsInitRand(pixbuf, width * height);

    while (spxeRun(pixbuf)) {
        
        if (spxeKeyPressed(ESCAPE) || spxeKeyPressed(Q)) {
            break;
        }
        
        if (spxeKeyPressed(R)) {
            pixelsInitRand(pixbuf, width * height);
        }
        
        spxeMousePos(&mousex, &mousey);

        pixelsUpdate(pixbuf, width, height);
        if (mousex >= 0 && mousex < width && mousey >= 0 && mousey < height) {
            memcpy(pixbuf + mousey * width + mousex, &red, sizeof(Px));
        }
    }
    
    return spxeEnd(pixbuf);
}
