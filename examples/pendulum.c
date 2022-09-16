#define SPXE_APPLICATION
#include <spxe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct vec2 {
    float x, y;
} vec2;

typedef struct ivec2 {
    int x, y;
} ivec2;

typedef struct bmp4 {
    int width, height;
    Px* pixbuf;
} bmp4;

static inline void plot(bmp4 bmp, Px color, ivec2 p)
{
    memcpy(bmp.pixbuf + (p.y * bmp.width) + p.x, &color, sizeof(Px));
}

static void plotLine(bmp4 bmp, const Px color, ivec2 p0, ivec2 p1)
{
    const int dx = abs(p1.x - p0.x);
    const int sx = p0.x < p1.x ? 1 : -1;
    const int dy = -abs(p1.y - p0.y);
    const int sy = p0.y < p1.y ? 1 : -1;
    
    int error = dx + dy;
    
    while (1) {
        plot(bmp, color, p0);
        if (p0.x == p1.x && p0.y == p1.y) break;
        int e2 = 2 * error;
        if (e2 >= dy) {
            if (p0.x == p1.x) break;
            error = error + dy;
            p0.x = p0.x + sx;
        }
        if (e2 <= dx) {
            if (p0.y == p1.y) break;
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
    const Px red = {255, 0, 0, 255}, green = {0, 255, 0, 255};
    bmp4 framebuffer = {width, height, pixbuf};

    const size_t size = width * height * sizeof(Px);
    const ivec2 center = {width / 2, height / 2};
    ivec2 p = {center.x + width / 8, center.y};
    vec2 xy = {(float)p.x, (float)p.y};
    float v = 0.0;

    const ivec2 idif = {p.x - center.x, p.y - center.y};
    const float dist = sqrtf(idif.x * idif.x + idif.y * idif.y);
    const float invDist = 1.0 / dist;

    float T = spxeTime();
    while (spxeRun(pixbuf)) {
        if (spxeKeyPressed(ESCAPE)) {
            break;
        }
       
        float t = spxeTime();
        float dT = t - T;
        T = t;
        dT *= 1.0;

        vec2 dif = {(float)(p.x - center.x), (float)(p.y - center.y)};
        vec2 cross = {dif.y * invDist, dif.x * invDist};
        
        v -= cross.y;
        float f = v * dT;
        xy.x -= cross.x * f, xy.y += cross.y * f;
        p.x = (int)xy.x, p.y = (int)xy.y;

        memset(pixbuf, 155, size);
        if (p.x >= 0 && p.y >= 0 && p.x < width && p.y < height) {
            ivec2 d = {(int)(xy.x - cross.x * v * 0.25), (int)(xy.y + cross.y * v * 0.25)};
            plotLine(framebuffer, red, center, p);
            plotLine(framebuffer, green, p, d);
        }

    }

    return spxeEnd(pixbuf);
}
