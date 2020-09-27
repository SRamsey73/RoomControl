#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Arduino.h>
#include <vector>
#include <string.h>

using std::vector;

class Peripheral
{
private:
	/**Intstantiated Peripherals**/
	//Vector to store derived classes
	static vector<Peripheral*> instantiatedPeripherals;


	/**REMOTE FUNCTIONS**/
	//Pointer to array of remoteFunctionNames
	const char** remoteFuncNames;
	const size_t numberOfRemoteFuncs;

protected:
	//Name of peripheral
	char name[30];


	/*UPDATE*/
	static unsigned long lastUpdateTime;
	virtual void onUpdate(const unsigned long* elapsedTime) = 0;


	/**REMOTE FUNCTIONS**/
	//Calls a remote function by its index, must be overrided by a derived class
	virtual bool callRemoteFunctionByIndex(size_t functionIndex, const char* callString) = 0;
	//Returns the index to a remote function based on its name
	int findRemoteFunctionIndex(const char* funcName);

public:
	/**CONSTRUCTOR / DESTRUCTOR**/
	Peripheral(const char* name, const char** remoteFuncNames, const size_t numberOfRemoteFuncs);
	~Peripheral();


	/*UPDATE*/
	static void update();


	/**REMOTE FUNCTIONS**/
	//Pass a complete callString to call a remote function, searches all instantiated peripherals
	static bool callRemoteFunction(const char* callString);
	void sendCallString(const char* funcName, const char* param);
	void sendCallString(const char* funcName, int param);
	void sendCallString(const char* funcName, float param);

	static void sendSerial(const char* msg);
};



