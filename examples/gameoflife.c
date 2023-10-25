#define SPXE_APPLICATION
#include <spxe.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define pxAt(px, w, x, y) (px[(((y) * (w)) + (x))].r)

static const Px black = {0, 0, 0, 255};
static const Px white = {255, 255, 255, 255};

static void pxInit(Px* pixbuf, const size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i) {
        memcpy(pixbuf + i, rand() % 2 ? &white : &black, sizeof(Px));
    }
}

static void pxUpdate(Px* pixbuf, Px* buf, const int width, const int height)
{
    int x, y, count, index = 0;
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x, ++index) {
            count = 0;
            count += (x + 1 < width) && pxAt(pixbuf, width, x + 1, y);
            count += (x + 1 < width) && (y > 0) && pxAt(pixbuf, width, x + 1, y - 1);
            count += (y > 0) && pxAt(pixbuf, width, x, y - 1);
            count += (x > 0) && (y > 0) && pxAt(pixbuf, width, x - 1, y - 1);
            count += (x > 0) && pxAt(pixbuf, width, x - 1, y);
            count += (x > 0) && (y + 1 < height) && pxAt(pixbuf, width, x - 1, y + 1);
            count += (y + 1 < height) && pxAt(pixbuf, width, x, y + 1);
            count += (x + 1 < width) && (y + 1 < height) && 
                        pxAt(pixbuf, width, x + 1, y + 1);

            buf[index] = pixbuf[index].r ?
                        (count == 2 || count == 3 ? white : black) : 
                        (count == 3 ? white : black);
        }
    }

    memcpy(pixbuf, buf, width * height * sizeof(Px));
}

int main(const int argc, char** argv)
{
    Px* pixbuf, *buf;
    const Px red = {255, 0, 0, 255};
    int mousex, mousey, width = 320, height = 240;
    
    if (argc > 1) {
        width = atoi(argv[1]);
        height = argc > 2 ? atoi(argv[2]) : width;
    }

    srand(time(NULL));
    pixbuf = spxeStart("game of life", 800, 600, width, height);
    buf = (Px*)malloc(width * height * sizeof(Px));
    pxInit(pixbuf, width * height);

    while (spxeRun(pixbuf)) {
        spxeMousePos(&mousex, &mousey);
        if (spxeKeyPressed(KEY_ESCAPE) || spxeKeyPressed(KEY_Q)) {
            break;
        }
        if (spxeKeyPressed(KEY_R)) {
            pxInit(pixbuf, width * height);
        }
        
        pxUpdate(pixbuf, buf, width, height);
        if (mousex >= 0 && mousex < width && mousey >= 0 && mousey < height) {
            pixbuf[mousey * width + mousex] = red;
        }
    }
    
    free(buf);
    return spxeEnd(pixbuf);
}

