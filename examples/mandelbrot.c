#include <stdlib.h>
#include <string.h>
#include <math.h>
#define SPXE_APPLICATION
#include <spxe.h>

#define ITERS 200
#define CLAMP(f) (double)(((f) > 1.0) + f * ((f) >= 0.0 && (f) <= 1.0))

static double zoom = 1.0;

typedef struct vec2 {
    double x, y;
} vec2;

static inline vec2 vec2_mult(const vec2 a, const vec2 b)
{
    return (vec2){a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x};
}

static inline vec2 vec2_add(const vec2 a, const vec2 b)
{
    return (vec2){a.x + b.x, a.y + b.y};
}

static inline double vec2_dot(const vec2 a, const vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

static void pxDraw(unsigned char* pixels, const int width, const int x, const int y, const double n, const double t)
{
    const double c[3] = {CLAMP(sin(t) * n * 0.8 + 0.2), CLAMP(sin(t * 0.333333) * n), CLAMP(cos(t * 0.7) * n * 0.8 + 0.2)};
    const unsigned char p[4] = {(unsigned)(c[0] * 255.0f), (unsigned)(c[1] * 255.0f), (unsigned)(c[2] * 255.0f), 255};
    memcpy(pixels + (y * width + x) * 4, p, 4);
}

static void pixelsUpdate(unsigned char* pixels, const int width, const int height, const double posx, const double posy, const double t)
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const vec2 p = {((double)x + posx) / ((double)width * zoom), ((double)y + posy) / ((double)height * zoom)};
            vec2 z = {0.0, 0.0};
            double n = 0.0;
            for (int i = 0; i < ITERS; ++i) {
                z = vec2_add(p, vec2_mult(z, z));
                double dot = vec2_dot(z, z);
                if (dot > 45678.0) {
                    n = ((double)i - (log2(dot)) + 4.0) / (double)ITERS;
                    break;
                }
            }
            pxDraw(pixels, width, x, y, n, t);
        }
    }
}

int main(const int argc, char** argv)
{
    int width = 64, height = 64; 
    if (argc > 1) {
        width = height = atoi(argv[1]);
    }
    if (argc > 2) {
        height = atoi(argv[2]);
    }
  
    unsigned char* pixels = spxeStart("Pixel Mandelbrot", 800, 600, width, height);
    double posx = 0.0, posy = 0.0, t = spxeTime();
    while (spxeRun(pixels)) { 
        double T = spxeTime();
        double dT = (T - t) * 50.0;
        t = T;

        if (spxeKeyPressed(ESCAPE) || spxeKeyPressed(Q)) {
            break;
        }
        if (spxeKeyDown(D)) {
            posx += dT / zoom;
        }
        if (spxeKeyDown(A)) {
            posx -= dT / zoom;
        }
        if (spxeKeyDown(W)) {
            posy += dT / zoom;
        }
        if (spxeKeyDown(S)) {
            posy -= dT / zoom;
        }
        if (spxeKeyDown(Z)) {
            zoom += dT * 0.01 * zoom;
        }
        if (spxeKeyDown(X)) {
            zoom -= dT * 0.01 * zoom;
        }
        pixelsUpdate(pixels, width, height, (posx + posx * zoom) * 0.5, (posy + posy * zoom) * 0.5, t);
    }
    return spxeEnd(pixels);
}
