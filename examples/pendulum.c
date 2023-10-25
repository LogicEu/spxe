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

static vec2 vec2_create(float x, float y)
{
    vec2 p;
    p.x = x;
    p.y = y;
    return p;
}

static ivec2 ivec2_create(int x, int y)
{
    ivec2 p;
    p.x = x;
    p.y = y;
    return p;
}

static void pxPlotLine(bmp4 bmp, ivec2 p0, ivec2 p1, Px color)
{
    const int dx = ABS(p1.x - p0.x);
    const int dy = -ABS(p1.y - p0.y);
    const int sx = p0.x < p1.x ? 1 : -1;
    const int sy = p0.y < p1.y ? 1 : -1;
    int e2, error = dx + dy;
    
    while (1) {
        bmp.pixbuf[p0.y * bmp.width + p0.x] = color;
        if (p0.x == p1.x && p0.y == p1.y) {
            break;
        }

        e2 = error * 2;
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
    const Px red = {255, 0, 0, 255}, green = {0, 255, 0, 255};
    
    size_t size;
    vec2 xy, dif, cross;
    ivec2 p, center, idif;
    bmp4 fb = {NULL, 200, 150};
    float invDist, dT, T, t, v = 0.0f;

    if (argc > 1) {
        fb.width = atoi(argv[1]);
        fb.height = argc > 2 ? atoi(argv[2]) : fb.width;
    }

    fb.pixbuf = spxeStart("Pendulum", 800, 600, fb.width, fb.height);
    size = fb.width * fb.height * sizeof(Px);
    center = ivec2_create(fb.width / 2, fb.height / 2);
    p = ivec2_create(center.x + fb.width / 8, center.y);
    xy = vec2_create((float)p.x, (float)p.y);

    idif = ivec2_create(p.x - center.x, p.y - center.y);
    invDist = 1.0F / sqrt(idif.x * idif.x + idif.y * idif.y);
    t = spxeTime();

    while (spxeRun(fb.pixbuf)) {
        T = spxeTime();
        dT = (T - t);
        t = T;

        if (spxeKeyPressed(KEY_ESCAPE)) {
            break;
        }

        dif = vec2_create((float)(p.x - center.x), (float)(p.y - center.y));
        cross = vec2_create(dif.y * invDist, dif.x * invDist);
        
        v -= cross.y;
        xy.x -= cross.x * v * dT;
        xy.y += cross.y * v * dT;
        p.x = (int)xy.x;
        p.y = (int)xy.y;

        memset(fb.pixbuf, 155, size);
        if (p.x >= 0 && p.y >= 0 && p.x < fb.width && p.y < fb.height) {
            ivec2 d = ivec2_create(
                (int)(xy.x - cross.x * v * 0.25F),
                (int)(xy.y + cross.y * v * 0.25F)
            );

            pxPlotLine(fb, center, p, red);
            pxPlotLine(fb, p, d, green);
        }
    }

    return spxeEnd(fb.pixbuf);
}

