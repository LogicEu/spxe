#define SPXE_APPLICATION
#include <spxe.h>

#define SIZE 256
#define WIDTH SIZE
#define HEIGHT SIZE

static void spxeDrawGradient(Px* pixbuf)
{
    int x, y;
    for (y = 0; y < HEIGHT; ++y) {
        for (x = 0; x < WIDTH; ++x) {
            Px px = {0, 0, 0, SIZE - 1};
            px.r = x;
            px.b = y;
            pixbuf[y * WIDTH + x] = px;
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

