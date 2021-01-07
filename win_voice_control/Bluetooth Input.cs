using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Room_Control__PC_
{
    class BluetoothInputHandler
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

        public BluetoothInputHandler(SerialPort bluetoothSerialPort)
        {
            Debug.WriteLine("Bluetooth Input Handler Started");
            mSerialPort = bluetoothSerialPort;
            mSerialPort.DataReceived += new SerialDataReceivedEventHandler(bluetoothDataReceived);
        }

        public void bluetoothDataReceived(object sender, SerialDataReceivedEventArgs e)
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
                            Debug.WriteLine("Bluetooth request to toggle temperature control");
                            mRoomInteractions.changeTempControlState(TOGGLE);
                            break;
                        case 73:
                            mRoomInteractions.updateAmbientTemp();
                            break;
                        case 74:
                            Debug.WriteLine("Bluetooth request to change target temp to: " + incomingData);
                            mRoomInteractions.changeTargetTemp(incomingData);
                            break;
                    }
                    if (incomingMeaning != 2)
                    {
                        if (dataLength > 0)
                        {
                            Debug.Write("BT: " + incomingMeaning + " | ");
                            Debug.WriteLine("Data: " + incomingData);
                        }
                        else
                        {
                            Debug.WriteLine("BT: " + incomingMeaning);
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
