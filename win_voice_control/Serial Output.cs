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
    public class SerialOutputWriter
    {
        SerialPort mSerialPort;
        public SerialOutputWriter(SerialPort port)
        {
            mSerialPort = port;
        }

        public void write(int meaning, int data)
        {

            byte[] writeBuffer = new byte[4];
            int outputLength = 0;
            int offset = 0;

            writeBuffer[0] = Convert.ToByte(meaning);

            writeBuffer[3] = (byte)(data & 0xFF);
            writeBuffer[2] = (byte)((data >> 8) & 0xFF);
            writeBuffer[1] = (byte)((data >> 16) & 0xFF);

            switch (meaning)
            {
                case 4:
                    outputLength = 1;
                    break;
                case 6:
                    outputLength = 1;
                    break;
                case 10:
                    outputLength = 0;
                    break;
                case 11:
                    outputLength = 0;
                    break;
                case 13:
                    outputLength = 1;
                    break;
                case 23:
                    outputLength = 1;
                    break;
                case 33:
                    outputLength = 1;
                    break;
                case 34:
                    outputLength = 1;
                    break;
                case 73:
                    outputLength = 1;
                    break;
                case 74:
                    outputLength = 1;
                    break;
            }
            switch (outputLength)
            {
                case 3:
                    offset = 0;
                    writeBuffer[0] = (byte)meaning;
                    break;
                case 2:
                    offset = 1;
                    writeBuffer[1] = (byte)meaning;
                    break;
                case 1:
                    offset = 2;
                    writeBuffer[2] = (byte)meaning;
                    break;
            }
            try
            {
                mSerialPort.Write(writeBuffer, offset, outputLength + 1);
                if (meaning != 2)
                {
                    Debug.WriteLine(mSerialPort.PortName + ": " + meaning + " | Data: " + data);
                }
                Thread.Sleep(20);
            }
            catch (Exception /*e*/)
            {
                //Debug.Write("Serial Port: " + mSerialPort.PortName + "Meaning: " + meaning + " ");
                //Debug.WriteLine(e.Message);
            }
        }
    }
}

