using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Diagnostics;

namespace Room_Control__PC_
{
    class ESP32APInput
    {
        const int OFF = 0;
        const int ON = 1;
        const int TOGGLE = 2;
        const int LIGHTS = 0;
        const int FAN = 1;

        RoomInteractions mRoomInteractions = Program.mRoomInteractions;

        int[] incomingByte = new int[10];
        int incomingMeaning;
        int incomingData = 0;
        int bytesRead = 0;
        int dataLength = 0;

        private SerialPort mSerialPort;

        public ESP32APInput(SerialPort esp32APSerial)
        {
            Debug.WriteLine("ESP32 AP Input Handler Started");
            mSerialPort = Program.esp32APSerialPort;
            mSerialPort.DataReceived += new SerialDataReceivedEventHandler(esp32APDataReceived);
        }

        public void esp32APDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            while (mSerialPort.BytesToRead > 0)
            {
                incomingByte[bytesRead] = mSerialPort.ReadByte();
                dataLength = 0;
                switch (incomingByte[0])
                {
                    //Define length of transmission here
                    case 4:
                        dataLength = 1;
                        break;
                    case 6:
                        dataLength = 1;
                        break;
                    case 12:
                        dataLength = 0;
                        break;
                    case 13:
                        dataLength = 1;
                        break;
                    case 22:
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
                            mRoomInteractions.syncBluetooth();
                            break;
                        case 2:
                            mRoomInteractions.pingBluetooth(1);
                            break;
                        case 4:
                            mRoomInteractions.ledSettings(incomingData);
                            break;
                        case 6:
                            mRoomInteractions.voiceSettings(incomingData);
                            break;
                        case 12:
                            Debug.WriteLine("Toggle light from ESP32");
                            mRoomInteractions.changeOverheadLightState(TOGGLE);
                            break;
                        case 13:
                            mRoomInteractions.dimOverheadLight(incomingData);
                            break;
                        case 22:
                            mRoomInteractions.changeFanState(TOGGLE);
                            break;
                        case 23:
                            mRoomInteractions.changeFanSpeed(incomingData);
                            break;
                        case 32:
                            mRoomInteractions.changePIRState(TOGGLE);
                            break;
                        case 33:
                            mRoomInteractions.changePIRTimeout(LIGHTS, incomingData);
                            break;
                        case 34:
                            mRoomInteractions.changePIRTimeout(FAN, incomingData);
                            break;
                        case 42:
                            mRoomInteractions.changeLDRState(TOGGLE);
                            break;
                        case 52:
                            mRoomInteractions.changeNightState(TOGGLE);
                            break;
                        case 60:
                            mRoomInteractions.changeDisplayState(OFF);
                            break;
                        case 70:
                            mRoomInteractions.changeTempControlState(OFF);
                            break;
                        case 71:
                            mRoomInteractions.changeTempControlState(ON);
                            break;
                        case 72:
                            mRoomInteractions.changeTempControlState(TOGGLE);
                            break;
                        case 73:
                            mRoomInteractions.updateAmbientTemp();
                            break;
                        case 74:
                            mRoomInteractions.changeTargetTemp(incomingData);
                            break;
                    }
                    if (incomingMeaning != 2)
                    {
                        if (dataLength > 0)
                        {
                            Debug.Write("AP: " + incomingMeaning + " | ");
                            Debug.WriteLine("Data: " + incomingData);
                        }
                        else
                        {
                            Debug.WriteLine("AP: " + incomingMeaning);
                        }
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
