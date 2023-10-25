#define SPXE_APPLICATION
#include <spxe.h>
#include <stdio.h>
#include <string.h>

/* This program opens up a 800 x 600 window in a 10:1 pixel scale.
 * Each frame it clears the pixel buffer to white, then plots the 
 * center pixel red and the pixel pointed by the mouse blue.    */

#define WIDTH 800
#define HEIGHT 600
#define SCALE 1 / 10

int main(void)
{
    int mouseX, mouseY;
    const int width = WIDTH * SCALE, height = HEIGHT * SCALE;
    const int halfWidth = width / 2, halfHeight = height / 2;
    const size_t buflen = width * height * sizeof(Px);
    const Px red = {255, 0, 0, 255}, blue = {0, 0, 255, 0};
    
    Px* pixbuf = spxeStart("example", WIDTH, HEIGHT, width, height);
    if (!pixbuf) {
        fprintf(stderr, "could not init spxe\n");
        return 1;
    }

    while (spxeRun(pixbuf)) {
        spxeMousePos(&mouseX, &mouseY);
        if (spxeKeyPressed(KEY_ESCAPE)) {
            break;
        }

        memset(pixbuf, 255, buflen);
        pixbuf[halfHeight * width + halfWidth] = red;
        if (mouseX >= 0 && mouseX < width && mouseY >= 0 && mouseY < height) {
            pixbuf[mouseY * width + mouseX] = blue;
        }
    }

    return spxeEnd(pixbuf);
}

