#define SPXE_APPLICATION
#include <spxe.h>

#define SIZE 256
#define WIDTH SIZE
#define HEIGHT SIZE

static void spxeDrawGradient(Px* pixbuf)
{
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            Px px = {x, 0, y, SIZE - 1};
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
