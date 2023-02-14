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
