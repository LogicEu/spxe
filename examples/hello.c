#define SPXE_APPLICATION
#include <spxe.h>

int main(void)
{
    Px* pixbuf = spxeStart("title", 800, 600, 100, 75);
    while (spxeRun(pixbuf)) {
        if (spxeKeyPressed(KEY_ESCAPE)) {
            break;
        }
    }
    return spxeEnd(pixbuf);
}

