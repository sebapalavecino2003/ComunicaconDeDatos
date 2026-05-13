#ifndef RGB_CONTROL_H
#define RGB_CONTROL_H

#include <Arduino.h>

struct RGBColor { uint8_t r, g, b; };

class RGBManager {
public:
    RGBManager(int pR, int pG, int pB);
    void begin();
    void setColor(RGBColor color);
    RGBColor getNextInSequence();
    
private:
    int pinR, pinG, pinB;
    int currentIndex = -1;
    static const RGBColor sequence[8];
};

#endif