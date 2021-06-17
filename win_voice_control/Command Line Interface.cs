using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using static Room_Control__PC_.RoomInteractions;

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
                    case "lights":
                        mRoomInteractions.changeOverheadLightState(State.TOGGLE);
                        break;
                    case "fan":
                        mRoomInteractions.changeFanState(State.TOGGLE);
                        break;
                    case "fan speed":
                        Console.Write("Enter fan speed: ");
                        FanSpeed? speed = null;
                        while (speed == null)
                        {
                            switch (Console.ReadLine().ToLower())
                            {
                                case "low":
                                    speed = FanSpeed.LOW;
                                    break;
                                case "medium":
                                    speed = FanSpeed.MEDIUM;
                                    break;
                                case "high":
                                    speed = FanSpeed.HIGH;
                                    break;
                                default:
                                    Console.Write("Invalid state, try again: ");
                                    break;
                            }
                        }
                        mRoomInteractions.changeFanSpeed((FanSpeed) speed);
                        break;
                    case "light level":
                        Console.Write("Enter Light Level: ");
                        command = Console.ReadLine();
                        mRoomInteractions.changeOverheadLightBrightness(int.Parse(command));
                        break;
                    case "night":
                        mRoomInteractions.changeNightState(RoomInteractions.State.TOGGLE);
                        break;
                    case "display":
                        mRoomInteractions.changeDisplayState(0);
                        break;
                    case "say":
                        Console.Write("Enter Phrase: ");
                        command = Console.ReadLine();
                        mVoiceRecognition.virtualAssistant.SpeakAsync(command);
                        break;
                    case "mic":
                        Program.mVoiceRecogniton.configureRecEngine();
                        Console.WriteLine("Input set to default audio device");
                        break;
                    case "clear":
                        Console.Clear();
                        printHeader();
                        break;
                    case "quit":
                        Program.roomPi.close();
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
