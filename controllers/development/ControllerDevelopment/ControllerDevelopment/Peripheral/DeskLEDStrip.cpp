#include "DeskLEDStrip.h"


DeskLEDStrip::DeskLEDStrip() : LEDStrip("desk_leds", numLEDs) {
    registerAnimations(deskAnimationMap);

    //FastLED.addLeds<LED_TYPE,led_data,color_order>(strip, numLEDs).setCorrection(TypicalLEDStrip); // initializes LED strip
    //Blink Border LEDS to indicate on state
    /*
    for (int i = 0; i < numLEDs; ++i) {
        strip[i] = CRGB::Green;
    }
    */
    //FastLED.show();
}

void DeskLEDStrip::ComputerSpeaking::updateAnimation()
{
    //cout << "ComputerSpeaking" << endl;
    /*
    int i = progress.loopCount;
        if((csp.leftTail + 1 + i) <= 45) {
            strip[sComputerSpeaking.leftTail + 1 + i].setRGB(255, 50, 0);
            strip[sComputerSpeaking.rightTail - 1 - i].setRGB(255, 50, 0);
        }
        if ((sComputerSpeaking.leftTail - sComputerSpeaking.tailLength - i) > 22) {
            strip[sComputerSpeaking.leftTail - (sComputerSpeaking.tailLength - 1) - i].setRGB(255, 50, 0);
            strip[sComputerSpeaking.rightTail + (sComputerSpeaking.tailLength - 2) + i].setRGB(255, 50, 0);
        }
        if (progress.loopCount >= 20) {
            return;
        }
        else {
            progress.loopCount++;
        }
        */
}

void DeskLEDStrip::RWBLayout::updateAnimation()
{
    //cout << "RWBLayout" << endl;
}
