#define SPXE_APPLICATION
#include <spxe.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ITERS 50
#define CLAMP(f) (double)(((f) > 1.0) + f * ((f) >= 0.0 && (f) <= 1.0))

typedef struct vec2 {
    double x, y;
} vec2;

static double zoom = 1.0;

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

static inline void pxDraw(Px* px, const double n, const double t)
{
    px->r = (unsigned)(CLAMP(sin(t) * n * 0.8 + 0.2) * 255.0);
    px->g = (unsigned)(CLAMP(sin(t * 0.333333) * n) * 255.0);
    px->b = (unsigned)(CLAMP(cos(t * 0.7) * n * 0.8 + 0.2) * 255.0);
}

static void pxUpdate(Px* pixbuf, const int width, const int height, vec2 pos, double t)
{
    pos.x = (pos.x + pos.x * zoom) * 0.5;
    pos.y = (pos.y + pos.y * zoom) * 0.5;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {   
            
            const vec2 p = {
                ((double)x + pos.x) / ((double)width * zoom), 
                ((double)y + pos.y) / ((double)height * zoom)
            };

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
            
            pxDraw(pixbuf + width * y + x, n, t);
        }
    }
}

int main(const int argc, char** argv)
{
    double t;
    Px* pixbuf;
    vec2 pos = {0.0, 0.0};
    int width = 64, height = 64;

    if (argc > 1) {
        width = height = atoi(argv[1]);
    }
    if (argc > 2) {
        height = atoi(argv[2]);
    }
  
    pixbuf = spxeStart("mandelbrot", 800, 600, width, height);
    memset(pixbuf, 255, width * height * sizeof(Px));
    t = spxeTime();

    while (spxeRun(pixbuf)) { 
        double dT, T = spxeTime();
        dT = (T - t) * 50.0;
        t = T;

        if (spxeKeyPressed(ESCAPE) || spxeKeyPressed(Q)) {
            break;
        }
        if (spxeKeyDown(D)) {
            pos.x += dT / zoom;
        }
        if (spxeKeyDown(A)) {
            pos.x -= dT / zoom;
        }
        if (spxeKeyDown(W)) {
            pos.y += dT / zoom;
        }
        if (spxeKeyDown(S)) {
            pos.y -= dT / zoom;
        }
        if (spxeKeyDown(Z)) {
            zoom += dT * 0.01 * zoom;
        }
        if (spxeKeyDown(X)) {
            zoom -= dT * 0.01 * zoom;
        }
        
        pxUpdate(pixbuf, width, height, pos, t);
    }

    return spxeEnd(pixbuf);
}
