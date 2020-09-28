#include "Peripheral.h"

vector<Peripheral*> Peripheral::instantiatedPeripherals;
unsigned long Peripheral::lastUpdateTime;

void Peripheral::update()
{
	unsigned long elapsedTime = micros() - lastUpdateTime;
	for (auto derived : instantiatedPeripherals) {
		derived->onUpdate(&elapsedTime);
	}
	lastUpdateTime = micros();
}


bool Peripheral::callRemoteFunctionByIndex(size_t functionIndex, const char* callString)
{
	return false;
}

int Peripheral::findRemoteFunctionIndex(const char* callString)
{
	for (unsigned int i = 0; i < numberOfRemoteFuncs; i++) {
		if (strstr(callString, remoteFuncNames[i]) == callString) {
			return i;
		}
	}
	return -1;
}

Peripheral::Peripheral(const char* name, const char** remoteFuncNames, const size_t numberOfRemoteFuncs)
	: remoteFuncNames(remoteFuncNames), numberOfRemoteFuncs(numberOfRemoteFuncs) {
	lastUpdateTime = micros();
	strcpy(this->name, name);
	instantiatedPeripherals.push_back(this);
}

Peripheral::~Peripheral() {

}

bool Peripheral::callRemoteFunction(const char* callString)
{
	//Check that passed string is not a null pointer
	if (callString == nullptr) {
		return false;
	}

	//Loop through all derived peripherals
	for (auto derived : instantiatedPeripherals) {
		//Check if the name of the peripheral matches the call, must be at the beginning of the call
		if (strstr(callString, derived->name) == callString) {
			//Found matching derived pheripheral for callString

			//Eliminate peripheral name from front of string
			//Check that string contains the character ':' and has characters after first ':'
			if (strchr(callString, ':') != nullptr && strlen(strchr(callString, ':')) >= 2) {
				callString = strchr(callString, ':') + 1;
			}
			else {
				//callString incomplete, peripheral name was found, but function name was missing, return
				return false;
			}

			//Check if derived peripheral has a matching function to the call
			//Loop through remoteFunction names
			for (unsigned int i = 0; i < derived->numberOfRemoteFuncs; i++) {
				//Check if callString contains remoteFuncName, must be at begging of string
				if (strstr(callString, derived->remoteFuncNames[i]) == callString) {
					//Found matching function call
					//Check that string contains the character ':' and has characters after first ':'
					if (strchr(callString, ':') != nullptr && strlen(strchr(callString, ':')) >= 2) {
						//Call the derived function by its index
						//Index is obtained by passing the callString to the derived class to return its index
						//callString is then passed to the call, but the function name is removed
						//return result of calling function by index
						return derived->callRemoteFunctionByIndex(derived->findRemoteFunctionIndex(callString), strchr(callString, ':') + 1);
					}
					break;
				}
			}
		}
	}
	return false;
}

void Peripheral::sendCallString(const char* funcName, const char* param)
{
	//Allocate a buffer to send message
	char* buff = new char[strlen(name) + strlen(funcName) + strlen(param) + 4];

	//Construct string
	strcpy(buff, name);
	strcat(buff, ":");
	strcat(buff, funcName);
	strcat(buff, ":");
	strcat(buff, param);

	//Send
	sendSerial(buff);

	delete[] buff;
}

void Peripheral::sendCallString(const char* funcName, int param)
{
	char paramStr[12];
	itoa(param, paramStr, 10);
	sendCallString(funcName, paramStr);
}

void Peripheral::sendSerial(const char* msg)
{
	Serial.write(msg);
	Serial.write('\4');
}
