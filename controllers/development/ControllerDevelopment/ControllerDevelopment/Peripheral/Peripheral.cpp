#include "Peripheral.h"

vector<Peripheral*> Peripheral::instantiatedPeripherals;
unsigned long Peripheral::lastUpdateTime;

void Peripheral::update()
{
	unsigned long elapsedTime = micros() - lastUpdateTime;
	lastUpdateTime = micros();
	for (auto derived : instantiatedPeripherals) {
		derived->onUpdate(elapsedTime);
	}
}


Peripheral::Peripheral(const char* name) {
	lastUpdateTime = micros();
	strcpy(this->name, name);
	instantiatedPeripherals.push_back(this);
}


Peripheral::~Peripheral() {

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
	_itoa(param, paramStr, 10);
	sendCallString(funcName, paramStr);
}

void Peripheral::registerRemoteFunctions(const RemoteFunctionMap& remoteFunctionMap)
{
	this->remoteFunctionMap = remoteFunctionMap;
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
			for (auto& item : derived->remoteFunctionMap) {
				//Check if callString contains remoteFuncName, must be at beginning of string
				if (strstr(callString, item.first) == callString) {
					//Found matching function call
					//Check that string contains the character ':' and has characters after first ':'
					if (strchr(callString, ':') != nullptr && strlen(strchr(callString, ':')) >= 2) {
						//Call the derived function
						((*derived).*(item.second))(strchr(callString, ':') + 1);
						return true;
					}
					break;
				}
			}
		}
	}
	return false;
}

void Peripheral::sendSerial(const char* msg)
{
	Serial.write(msg);
	Serial.write('\4');
}
