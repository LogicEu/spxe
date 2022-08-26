#define SPXE_APPLICATION
#include <spxe.h>

int main(void)
{
    const int width = 256, height = 256;
    Px* pixbuf = spxeStart("Pixel Gradient", 800, 600, width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Px px = {x, 0, y, 255};
            spxePxSet(spxePxGet(pixbuf, x, y), px);
        }
    }

    while (spxeRun(pixbuf)) {
        if(spxeKeyPressed(ESCAPE)) {
            break;
        }
    }

    return spxeEnd(pixbuf);
}
