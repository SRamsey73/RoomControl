using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Net;
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
        public static VoiceRecognitionHandler mVoiceRecogniton;
        public static RoomControllerInterface roomPi;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>

        [STAThread]

        [DllImport("kernel32.dll")]
        static extern IntPtr GetConsoleWindow();

        [DllImport("user32.dll")]
        public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
        public static IntPtr console = GetConsoleWindow();

        public const int SW_HIDE = 0, SW_SHOW = 5;

        static void Main(string[] args)
        {
            //ShowWindow(console, SW_HIDE);
            AppDomain.CurrentDomain.ProcessExit += new EventHandler(OnProcessExit);
            //SystemEvents.PowerModeChanged += OnPowerChange;
            Console.Clear();
            CommandLineInterface.printHeader();
            Console.Write("Program Initializing...");
            Debug.WriteLine("Program Starting");
            mRoomInteractions = new RoomInteractions();
            Thread.Sleep(350);
            roomPi = new RoomControllerInterface("Room Pi", new IPEndPoint(IPAddress.Parse("169.254.44.49"), 7500));
            startVoiceRecognitionHandler();
            Console.Write("DONE\n");
            startCommandLineInterface();
            while(true);
        }

        static void OnProcessExit(object sender, EventArgs eventArgs)
        {
            Console.WriteLine("Exiting");
            Thread.Sleep(1000);
        }

        static void startCommandLineInterface()
        {
            CommandLineInterface mCommandLineInterface = new CommandLineInterface();
        }
        static void startVoiceRecognitionHandler()
        {
            mVoiceRecogniton = new VoiceRecognitionHandler();
        }

        private static void OnPowerChange(object s, PowerModeChangedEventArgs e)
        {
            switch (e.Mode)
            {
                case PowerModes.Resume:
                    Console.WriteLine("Resume");
                    mVoiceRecogniton.configureRecEngine();
                    break;
                case PowerModes.Suspend:
                    break;
            }
        }

    }
}
