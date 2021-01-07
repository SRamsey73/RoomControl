#include "LEDStrip.h"

//Animations for desk ledStrip
struct TailEffect : LEDStrip::Animation {
    const int tailLength = 20;
    const int fadeLength = 3;
    int tailPosition;
    int tailColor = 0;
    int previousTailColor = 0;
    int leftTail = 0;
    int rightTail = 0;
    
    TailEffect(CRGB* strip, int numLEDs);
    void updateAnimation() override;
};

struct RWBCenter : LEDStrip::Animation
{
    int position = 0;
    int direction = 1;
    int laying = 2;
    int color = 3;
    
    RWBCenter(CRGB* strip, int numLEDs);
    void updateAnimation() override;
};