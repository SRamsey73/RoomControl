using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Room_Control__PC_
{
    class BasementArduinoSerialInputHandler
    {
        RoomInteractions mRoomInteractions = Program.mRoomInteractions;
        GlobalVariables mGlobalVariables = Program.mGlobalVariables;

        const int OFF = 0;
        const int ON = 1;
        const int TOGGLE = 2;

        int[] incomingByte = new int[10];
        int incomingMeaning;
        int incomingData = 0;
        int bytesRead = 0;
        int dataLength = 0;
   
        public SerialPort arduinoSerial = Program.basementArduinoSerialPort;

        public BasementArduinoSerialInputHandler() {
            Debug.WriteLine("Basment Arduino Input Handler Started");
            arduinoSerial.DataReceived += new SerialDataReceivedEventHandler(arduinoSerialDataReceived);
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
                    case 10:
                        dataLength = 0;
                        break;
                    case 11:
                        dataLength = 0;
                        break;
                    case 13:
                        dataLength = 1;
                        break;
                    case 23:
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
                    switch (incomingMeaning)
                    {
                        case 10:
                            mRoomInteractions.syncOverheadLightState(OFF);
                            //Debug.WriteLine("LIGHTS OFF");
                            break;
                        case 11:
                            mRoomInteractions.syncOverheadLightState(1);
                            //Debug.WriteLine("LIGHTS ON");
                            break;
                        case 13:
                            mRoomInteractions.syncOverheadLightBrightness(incomingData);
                            break;
                        case 20:
                            mRoomInteractions.syncFanState(OFF);
                            //Debug.WriteLine("FAN OFF");
                            break;
                        case 21:
                            mRoomInteractions.syncFanState(ON);
                            //Debug.WriteLine("FAN ON");
                            break;
                        case 23:
                            mRoomInteractions.syncFanSpeed(incomingData);
                            break;
                    }
                    if (dataLength > 0)
                    {
                        Debug.Write("BA: " + incomingMeaning + " | ");
                        Debug.WriteLine("Data: " + incomingData);
                    }
                    else
                    {
                        Debug.WriteLine("BA: " + incomingMeaning);
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
