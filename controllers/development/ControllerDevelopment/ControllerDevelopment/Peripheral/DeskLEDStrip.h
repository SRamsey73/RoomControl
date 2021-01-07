#pragma once

#include "LEDStrip.h"

class DeskLEDStrip : public LEDStrip {
private:
    //LED options
    static const int numLEDs = 90, brightness = 200, section_length = 2, led_data = 47;
    //static const EOrder color_order = GRB;

    /***ANIMATIONS***/
    //Computer Speaking
    struct ComputerSpeaking : Animation {
        const int tailLength = 3;
        int leftTail = 0;
        int rightTail = 0;
        ComputerSpeaking() : Animation(50 * 1000) {};
        void updateAnimation() override;
    };

    struct RWBLayout : Animation
    {
        void updateAnimation() override;
    };

    AnimationMap deskAnimationMap = {
        { "computer_speaking", new ComputerSpeaking() },
        { "rwb_layout", new RWBLayout() }
    };

public:
    DeskLEDStrip();

};