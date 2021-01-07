using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Room_Control__PC_
{
    class CommandLineInterface
    {
        //Import room interactions for use in command line
        RoomInteractions mRoomInteractions = Program.mRoomInteractions;
        VoiceRecognitionHandler mVoiceRecognition = Program.mVoiceRecogniton;

        //Constants
        public const int OFF = 0;
        public const int ON = 1;
        public const int TOGGLE = 2;
        public const int LIGHTS = 0;
        public const int FAN = 1;

        //Variables
        String command;
        String input;

        public CommandLineInterface() {
            Thread commandInputThread = new Thread(commandLineInput);
            commandInputThread.Start();
        }

        public void commandLineInput()
        {
            //Print header
            while(true)
            {
                //Prompt user for command input
                Console.Write(">");
                command = Console.ReadLine();
                command.Split(' ');
                //Deterime meaning of command
                switch (command)
                {
                    case "sync":
                        mRoomInteractions.syncAll();
                        break;
                    case "lights":
                        mRoomInteractions.changeOverheadLightState(TOGGLE);
                        break;
                    case "fan":
                        mRoomInteractions.changeFanState(TOGGLE);
                        break;
                    case "fan speed":
                        Console.Write("Enter fan speed: ");
                        int speed = Convert.ToInt32(Console.ReadLine());
                        mRoomInteractions.changeFanSpeed(speed);
                        break;
                    case "light level":
                        Console.Write("Enter Light Level: ");
                        command = Console.ReadLine();
                        mRoomInteractions.dimOverheadLight(255 * (Convert.ToInt32(command))/100);
                        break;
                    case "night":
                        mRoomInteractions.changeNightState(2);
                        break;
                    case "display":
                        mRoomInteractions.changeDisplayState(0);
                        break;
                    case "say":
                        Console.Write("Enter Phrase: ");
                        command = Console.ReadLine();
                        mVoiceRecognition.virtualAssistant.SpeakAsync(command);
                        break;
                    case "clear":
                        Console.Clear();
                        printHeader();
                        break;
                    case "exit":
                        System.Environment.Exit(1);
                        break;
                    default:
                        Console.Write("Unknown Command\n");
                        break;
                }
            }
        }

        public static void printHeader()
        {
            Console.Write("Room Control Command Line Interface v1.0");
            Console.Write("\n----------------------------------------\n");
        }
    }
}
