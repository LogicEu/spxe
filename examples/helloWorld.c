#define SPXE_APPLICATION
#include <spxe.h>

int main(void)
{
    const int windowWidth = 800, windowHeight = 600;
    const int screenWidth = 200, screenHeight = 150;
    
    unsigned char* pixels = spxeStart(
        "Hello World", 
        windowWidth, 
        windowHeight, 
        screenWidth, 
        screenHeight
    );
    
    while (spxeRun(pixels)) {
        if (spxeKeyPressed(ESCAPE)) {
            break;
        }
    }
    
    return spxeEnd(pixels);
}
