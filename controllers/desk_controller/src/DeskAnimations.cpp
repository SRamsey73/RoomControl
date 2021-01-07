#include "DeskAnimations.h"

TailEffect::TailEffect(CRGB* strip, int numLEDs) : Animation(strip, numLEDs, 30) {}

void TailEffect::updateAnimation() {
    if (firstRun) {
		tailPosition = tailLength - 1;
		tailColor = 0;
	}
    if (tailPosition == 0) {
        previousTailColor = tailColor;
        if (tailColor == 2) {
            tailColor = 0;
        }
        else {
            tailColor++;
        }
    }
    for (int i = 0; i < numLEDs; i++) {
        switch (tailColor)
        {
        case 0:
            strip[i].setRGB(255, 0, 0);
            break;
        case 1:
            strip[i].setRGB(255, 255, 255);
            break;
        case 2:
            strip[i].setRGB(0, 0, 255);
            break;
        }

        // Dim leading and trailing edge of tail
        int delta = fadeLength + 1;
        if(i <= tailPosition && i >= tailPosition - fadeLength) {
            delta = tailPosition - i;
        }
        else if(i >= tailPosition - tailLength && i <= tailPosition - tailLength + fadeLength) {
            delta = i - (tailPosition - tailLength); 
        }
        else if(tailPosition < tailLength && i >= numLEDs - (tailLength - tailPosition) && i <= numLEDs - (tailLength - tailPosition) + fadeLength) {
            delta = i - (numLEDs - (tailLength - tailPosition));
        }
        if(delta <= fadeLength) {
            delta = fadeLength - delta + 1;
            delta *= 5;
            strip[i].r /= delta;
            strip[i].g /= delta;
            strip[i].b /= delta;
        }

        if (tailPosition < numLEDs - tailLength) {
            if(i > numLEDs - tailLength) {
                switch (previousTailColor)
                {
                case 0:
                    strip[i].setRGB(255, 0, 0);
                    break;
                case 1:
                    strip[i].setRGB(255, 255, 255);
                    break;
                case 2:
                    strip[i].setRGB(0, 0, 255);
                    break;
                }
            }
        }
    }
    for (int i = 0; i < numLEDs; i++) {
        if (tailPosition < tailLength - 1) {
            if (!(i <= tailPosition || i >= (numLEDs - 1) - (tailLength - (tailPosition + 2)))) {
                strip[i].setRGB(0, 0, 0);
            }
        }
        else if(!(i <= tailPosition && i > tailPosition - tailLength)) {
            strip[i].setRGB(0,0,0);
        }
    }
    if (tailPosition >= numLEDs - 1) {
        tailPosition = 0;
    }
    else {
        tailPosition++;
    }
}


RWBCenter::RWBCenter(CRGB* strip, int numLEDs) : Animation(strip, numLEDs, 30) {}

void RWBCenter::updateAnimation()
{
	if (firstRun) {
		position = 0;
		direction = 1;
		laying = 1;
		color = 0; 
		for (int i = 0; i < numLEDs; i++) {
			strip[i].setRGB(0, 0, 0);
		}
	}

    if (laying == 1) {
        switch (color)
        {
        case 0:
            strip[position].setRGB(255, 0, 0);
            strip[numLEDs - position - 1].setRGB(255, 0, 0);
            break;
        case 1:
            strip[position].setRGB(255, 255, 255);
            strip[numLEDs - position - 1].setRGB(255, 255, 255);
            break;
        case 2:
            strip[position].setRGB(0, 0, 255);
            strip[numLEDs - position - 1].setRGB(0, 0, 255);
            break;
        }
    }
    else {
        strip[position].setRGB(0, 0, 0);
        strip[numLEDs - position - 1].setRGB(0, 0, 0);
    }
    position += direction;
    if (position < 0 || position > 45) { //Reached end, change parameters
        if (laying == 0) { //If leds were being deleted, turn around and lay new colored ones
            laying = 1;
            if (position < 0) {
                if (color == 2) {
                    color = 0;
                }
                else {
                    color++;
                }
            }
            if (direction == 1) {
                direction = -1;
                position = 45;
            }
            else {
                direction = 1;
                position = 0;
            }
        }
        else if (laying == 1) { //If leds were being added, go to other end and delete them
            laying = 0;
            if (position >= 45) {
                position = 0;
            }
            else {
                position = 45;
            }
        }
    }	
}
