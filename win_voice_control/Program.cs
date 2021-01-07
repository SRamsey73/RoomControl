using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Room_Control__PC_
{
    public class Program 
    {
        public static RoomInteractions mRoomInteractions;
        public static GlobalVariables mGlobalVariables = new GlobalVariables();
        public static SerialPort basementArduinoSerialPort = new SerialPort();
        public static SerialPort deskArduinoSerialPort = new SerialPort();
        public static SerialPort esp32APSerialPort = new SerialPort();
        //public static SerialPort phoneSerialPort = new SerialPort();
        public static VoiceRecognitionHandler mVoiceRecogniton;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>

        [STAThread]
        static void Main(string[] args)
        {
            Console.Clear();
            CommandLineInterface.printHeader();
            Console.Write("Program Initializing...");
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Debug.WriteLine("Program Starting");
            configureSerialPorts();
            mRoomInteractions = new RoomInteractions();
            startBasementArduinoSerialInputHandler();
            startDeskArduinoSerialInputHanlder();
            //startBluetoothInputHandler();
            startAPInputHandler();
            Thread.Sleep(350);
            mRoomInteractions.syncForm();
            Thread.Sleep(500);
            mRoomInteractions.syncDeskArduino();
            Thread.Sleep(250);
            startVoiceRecognitionHandler();
            Console.Write("DONE\n");
            startCommandLineInterface();
            //Application.Run(new Form1());
        }

        static void startCommandLineInterface()
        {
            CommandLineInterface mCommandLineInterface = new CommandLineInterface();
            //Thread commandLineInterfaceThread = new Thread(CommandLineInterface.commandLineInput);
            //commandLineInterfaceThread.Start();
        }
        static void startVoiceRecognitionHandler()
        {
            mVoiceRecogniton = new VoiceRecognitionHandler();
        }
        static void startBasementArduinoSerialInputHandler()
        {
            BasementArduinoSerialInputHandler mBasementArduinoSerialInput = new BasementArduinoSerialInputHandler();
        }
        static void startDeskArduinoSerialInputHanlder()
        {
            DeskArduinoSerialInputHandler mDeskArduinoSerialInputHandler = new DeskArduinoSerialInputHandler();
        }
        static void startAPInputHandler()
        {
            ESP32APInput mESP32APInput = new ESP32APInput(esp32APSerialPort);
        }

        static void configureSerialPorts()
        {
            basementArduinoSerialPort.PortName = "COM3";
            esp32APSerialPort.PortName = "COM5";
            deskArduinoSerialPort.PortName = "COM6";
            basementArduinoSerialPort.BaudRate = 9600;
            deskArduinoSerialPort.BaudRate = 9600;
            esp32APSerialPort.BaudRate = 9600;
            basementArduinoSerialPort.WriteTimeout = 5;
            while (!basementArduinoSerialPort.IsOpen)
            {
                try
                {
                    basementArduinoSerialPort.Open();
                }
                catch (UnauthorizedAccessException)
                {

                }
            }
            try
            {
                deskArduinoSerialPort.Open();
            }
            catch(Exception)
            {

            }
            try
            {
                esp32APSerialPort.Open();
            }
            catch(Exception)
            {

            }
        }
        public static void displayOff()
        {

        }
    }
}
