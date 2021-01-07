// ControllerDevelopment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Peripheral/Peripheral.h"
#include "Peripheral/Fan.h"
#include "Peripheral/Light.h"
#include "Peripheral/DeskLEDStrip.h"
#include <thread>
#include <chrono>


void input()
{
    while (true) {
        std::string input;
        std::cin >> input;
        Peripheral::callRemoteFunction(input.c_str());
    }
}


int main()
{
    const uint8_t fan_pins[4] = { 6, 5, 4, 3 };
    Fan fan("fan", Fan::MULTI_SPEED_TYPE, fan_pins);
    Light light("light", Light::DIMMER_TYPE, 7);
    DeskLEDStrip deskLEDStrip;


    std::thread inputThread(input);

    while (true) {
        Peripheral::update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    system("pause");
}
