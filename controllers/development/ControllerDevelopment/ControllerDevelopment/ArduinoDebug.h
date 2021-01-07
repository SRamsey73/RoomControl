#pragma once

#include <iostream>
#include<chrono>

#define OUTPUT 1
#define INPUT 0

#define LOW 0
#define HIGH 1

using std::cout;
using std::cin;
using std::endl;

const std::chrono::steady_clock::time_point programStart = std::chrono::steady_clock::now();

inline unsigned long micros()
{
	auto now = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(now - programStart).count();
}


inline void pinMode(int pin, int direction)
{

}


inline int analogRead(int pin)
{
	//Prompt user for analog input
	return 700;
}

inline int digitalRead(int pin)
{
	//Prompt user for digital input
	return 0;
}

inline void digitalWrite(int pin, int value)
{

}

inline void analogWrite(int pin, int value)
{

}

class
{
public:
	void write(const char* msg)
	{
		std::cout << msg << std::endl;
	}
	void write(char)
	{

	}
} Serial;