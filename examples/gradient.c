#define SPXE_APPLICATION
#include <spxe.h>

#define WIDTH 256
#define HEIGHT 256

static Px spxeGradientAt(int x, int y)
{
    Px px = {0, 0, 0, 255};
    px.r = x;
    px.b = y;
    return px;
}

static void spxeDrawGradient(Px* pixbuf)
{
    int x, y;
    for (y = 0; y < HEIGHT; ++y) {
        for (x = 0; x < WIDTH; ++x) {
            pixbuf[y * WIDTH + x] = spxeGradientAt(x, y);
        }
    }
}

int main(void)
{
    Px* pixbuf = spxeStart("gradient", 800, 600, WIDTH, HEIGHT);
    spxeDrawGradient(pixbuf);
    while (spxeRun(pixbuf)) {
        if(spxeKeyPressed(ESCAPE)) {
            break;
        }
    }
    return spxeEnd(pixbuf);
}

