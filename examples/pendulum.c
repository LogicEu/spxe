#define SPXE_APPLICATION
#include <spxe.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ABS(n) ((n) > 0 ? (n) : -(n))

typedef struct vec2 {
    float x, y;
} vec2;

typedef struct ivec2 {
    int x, y;
} ivec2;

typedef struct bmp4 {
    Px* pixbuf;
    int width, height;
} bmp4;

static void pxPlotLine(bmp4 bmp, ivec2 p0, ivec2 p1, Px color)
{
    const int dx = ABS(p1.x - p0.x);
    const int dy = -ABS(p1.y - p0.y);
    const int sx = p0.x < p1.x ? 1 : -1;
    const int sy = p0.y < p1.y ? 1 : -1;
    
    int error = dx + dy;
    
    while (1) {
        bmp.pixbuf[p0.y * bmp.width + p0.x] = color;
        if (p0.x == p1.x && p0.y == p1.y) {
            break;
        }

        int e2 = error * 2;
        if (e2 >= dy) {
            if (p0.x == p1.x) {
                break;
            }

            error = error + dy;
            p0.x = p0.x + sx;
        }

        if (e2 <= dx) {
            if (p0.y == p1.y) {
                break;
            }

            error = error + dx;
            p0.y = p0.y + sy;
        }
    }
}

int main(const int argc, const char** argv)
{
    int width = 200, height = 150;
    if (argc > 1) {
        width = height = atoi(argv[1]);
    }
    if (argc > 2) {
        height = atoi(argv[2]);
    }

    Px* pixbuf = spxeStart("Pendulum", 800, 600, width, height);
    bmp4 framebuffer = {pixbuf, width, height};
    
    const Px red = {255, 0, 0, 255}, green = {0, 255, 0, 255};
    const size_t size = width * height * sizeof(Px);
    const ivec2 center = {width / 2, height / 2};
    ivec2 p = {center.x + width / 8, center.y};
    vec2 xy = {(float)p.x, (float)p.y};

    const ivec2 idif = {p.x - center.x, p.y - center.y};
    const float invDist = 1.0F / sqrtf(idif.x * idif.x + idif.y * idif.y);
    float v = 0.0F, t = spxeTime();

    while (spxeRun(pixbuf)) {
        double dT, T = spxeTime();
        dT = (T - t);
        t = T;

        if (spxeKeyPressed(ESCAPE)) {
            break;
        }

        vec2 dif = {(float)(p.x - center.x), (float)(p.y - center.y)};
        vec2 cross = {dif.y * invDist, dif.x * invDist};
        
        v -= cross.y;
        xy.x -= cross.x * v * dT;
        xy.y += cross.y * v * dT;
        p.x = (int)xy.x;
        p.y = (int)xy.y;

        memset(pixbuf, 155, size);
        if (p.x >= 0 && p.y >= 0 && p.x < width && p.y < height) {
            ivec2 d = {
                (int)(xy.x - cross.x * v * 0.25F),
                (int)(xy.y + cross.y * v * 0.25F)
            };

            pxPlotLine(framebuffer, center, p, red);
            pxPlotLine(framebuffer, p, d, green);
        }
    }

    return spxeEnd(pixbuf);
}
