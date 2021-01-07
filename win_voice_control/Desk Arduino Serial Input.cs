using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Room_Control__PC_
{
    class DeskArduinoSerialInputHandler
    {
        RoomInteractions mRoomInteractions = Program.mRoomInteractions;
        GlobalVariables mGlobalVariables = Program.mGlobalVariables;

        const int OFF = 0;
        const int ON = 1;
        const int TOGGLE = 2;
        const int LIGHTS = 0;
        const int FAN = 1;

        int[] incomingByte = new int[10];
        int incomingMeaning;
        int incomingData = 0;
        int bytesRead = 0;
        int dataLength = 0;

        public SerialPort arduinoSerial = Program.deskArduinoSerialPort;

        public DeskArduinoSerialInputHandler()
        {
            Debug.WriteLine("Desk Arduino Input Handler Started");
            arduinoSerial.DataReceived += new SerialDataReceivedEventHandler(arduinoSerialDataReceived);
            //mRoomInteractions.syncDeskArduino();
        }

        public void arduinoSerialDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            while (arduinoSerial.BytesToRead > 0)
            {
                incomingByte[bytesRead] = arduinoSerial.ReadByte();
                dataLength = 0;
                switch (incomingByte[0])
                {
                    //Define length of transmission here
                    case 4:
                        dataLength = 1;
                        break;
                    case 12:
                        dataLength = 0;
                        break;
                    case 23:
                        dataLength = 1;
                        break;
                    case 33:
                        dataLength = 1;
                        break;
                    case 34:
                        dataLength = 1;
                        break;
                    case 73:
                        dataLength = 1;
                        break;
                    case 74:
                        dataLength = 1;
                        break;
                }
                switch (bytesRead)
                {
                    case 1:
                        incomingData = incomingByte[1];
                        break;
                    case 2:
                        incomingData = (incomingData << 8) + incomingByte[2];
                        break;
                    case 3:
                        incomingData = (incomingData << 8) + incomingByte[3];
                        break;
                }
                if (bytesRead == dataLength)
                {
                    incomingMeaning = incomingByte[0];
                    //Define meaning of transmission here
                    switch (incomingByte[0])
                    {
                        case 1:
                            mRoomInteractions.syncDeskArduino();
                            break;
                        case 4:
                            mRoomInteractions.syncLEDSettings(incomingData);
                            break;
                        case 7:
                            mRoomInteractions.playGreeting();
                            break;
                        case 10:
                            mRoomInteractions.changeOverheadLightState(OFF);
                            break;
                        case 11:
                            mRoomInteractions.changeOverheadLightState(ON);
                            break;
                        case 12:
                            mRoomInteractions.changeOverheadLightState(TOGGLE);
                            break;
                        case 14:
                            mRoomInteractions.occupancyWarning();
                            break;
                        case 20:
                            mRoomInteractions.changeFanState(OFF);
                            break;
                        case 21:
                            mRoomInteractions.changeFanState(ON);
                            break;
                        case 22:
                            mRoomInteractions.changeFanSpeed(TOGGLE);
                            break;
                        case 23:
                            mRoomInteractions.changeFanSpeed(incomingData);
                            break;
                        case 30:
                            mRoomInteractions.syncPIRState(OFF);
                            break;
                        case 31:
                            mRoomInteractions.syncPIRState(ON);
                            break;
                        case 33:
                            mRoomInteractions.syncPIRTimeout(LIGHTS, incomingData);
                            break;
                        case 34:
                            mRoomInteractions.syncPIRTimeout(FAN, incomingData);
                            break;
                        case 40:
                            mRoomInteractions.syncLDRState(OFF);
                            break;
                        case 41:
                            mRoomInteractions.syncLDRState(ON);
                            break;
                        case 70:
                            mRoomInteractions.syncTempControl(OFF);
                            break;
                        case 71:
                            mRoomInteractions.syncTempControl(ON);
                            break;
                        case 73:
                            mRoomInteractions.syncAmbientTemp(incomingData);
                            break;
                        case 74:
                            mRoomInteractions.syncTargetTemp(incomingData);
                            break;
                    }
                    if (dataLength > 0)
                    {
                        Debug.Write("DA: " + incomingMeaning + " | ");
                        Debug.WriteLine("Data: " + incomingData);
                    }
                    else
                    {
                        Debug.WriteLine("DA: " + incomingMeaning);
                    }
                    incomingData = 0;
                    bytesRead = 0;
                    for (int i = 0; i < 10; i++)
                    {
                        incomingByte[i] = 0;
                    }
                    break;
                }
                bytesRead++;
            }
        }
    }
}

