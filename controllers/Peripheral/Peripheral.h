#pragma once
#define _CRT_SECURE_NO_WARNINGS

//Header for windows development
#ifdef WIN32
#include "../ArduinoDebug.h"
#else
//Header for arduino
#include <Arduino.h>
#define _itoa itoa
#endif

#include <vector>
#include <map>
#include <string.h>

using std::vector;

class Peripheral
{
private:
	/**Intstantiated Peripherals**/
	//Vector to store derived classes
	static vector<Peripheral*> instantiatedPeripherals;

protected:
	//Name of peripheral
	char name[30];


	/***LOCAL UPDATE***/
private:
	static unsigned long lastUpdateTime;
protected:
	virtual void onUpdate(unsigned long elapsedTime) = 0;

	/***GLOBAL(STATIC) UPDATE***/
public:
	static void update();


	/**REMOTE FUNCTIONS**/
protected:
	typedef void (Peripheral::* RemoteFunction)(const char*);
	typedef std::map<const char*, RemoteFunction> RemoteFunctionMap;
protected:
	RemoteFunctionMap remoteFunctionMap;
public:
	void registerRemoteFunctions(const RemoteFunctionMap& remoteFunctionMap);
	static bool callRemoteFunction(const char* callString);
	void sendCallString(const char* funcName, const char* param);
	void sendCallString(const char* funcName, int param);

	static void sendSerial(const char* msg);

public:
	/**CONSTRUCTOR / DESTRUCTOR**/
	Peripheral(const char* name);
	~Peripheral();
};



