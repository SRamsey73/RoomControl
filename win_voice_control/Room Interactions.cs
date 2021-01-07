using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Room_Control__PC_
{

    public class RoomInteractions
    {
        //Constants=================================================================================================
        public const int OFF = 0;
        public const int ON = 1;
        public const int TOGGLE = 2;
        public const int LIGHTS = 0;
        public const int FAN = 1;

        //Display Interactions======================================================================================
        const int WM_SYSCOMMAND = 0x112;
        const int SC_MONITORPOWER = 0xF170;
        [DllImport("user32.dll")]
        public static extern void keybd_event(byte virtualKey, byte scanCode, uint flags, IntPtr extraInfo);
        private const int VK_SHIFT = 0x10;
        public const int KEYEVENTF_EXTENTEDKEY = 1;
        public const int KEYEVENTF_KEYUP = 0;

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, IntPtr wParam, IntPtr lParam);
        
        //Objects===================================================================================================
        GlobalVariables mGlobalVariables = Program.mGlobalVariables;

        public SerialOutputWriter basementArduinoSerial = new SerialOutputWriter(Program.basementArduinoSerialPort);
        SerialOutputWriter deskArduinoSerial = new SerialOutputWriter(Program.deskArduinoSerialPort);
        SerialOutputWriter esp32APSerial = new SerialOutputWriter(Program.esp32APSerialPort);

        public RoomInteractions()
        {
            Debug.WriteLine("RoomInteractions");
        }

        public void syncAll()
        {
            syncForm();
            syncDeskArduino();
            syncBluetooth();
        }

        public void syncForm()
        {
            basementArduinoSerial.write(1, 0);
        }

        public void syncDeskArduino()
        {
            deskArduinoSerial.write(mGlobalVariables.overheadLightState + 10, 0);
            deskArduinoSerial.write(mGlobalVariables.nightState + 50, 0);
            deskArduinoSerial.write(mGlobalVariables.pirState + 30, 0);
            deskArduinoSerial.write(mGlobalVariables.ldrState + 40, 0);
            deskArduinoSerial.write(4, Convert.ToInt32(mGlobalVariables.ledSettings));
            deskArduinoSerial.write(mGlobalVariables.tempControl + 70, 0);
            deskArduinoSerial.write(73, 0);
            deskArduinoSerial.write(74, mGlobalVariables.targetTemperature);
            Debug.WriteLine("Sync Desk Arduino");
            //deskArduinoSerial.write(Convert.ToInt32(fanState) + 20, 0);
        }

        public void syncBluetooth()
        {
            esp32APSerial.write(mGlobalVariables.overheadLightState + 10, 0);
            esp32APSerial.write(13, mGlobalVariables.overheadLightBrightness);
            esp32APSerial.write(mGlobalVariables.fanState + 20, 0);
            esp32APSerial.write(23, mGlobalVariables.fanSpeed);
            esp32APSerial.write(mGlobalVariables.pirState + 30, 0);
            //panel1Serial.write(33, mGlobalVariables.pirLightTimeout);
            //panel1Serial.write(34, mGlobalVariables.pirFanTimeout);
            esp32APSerial.write(33, 35);
            esp32APSerial.write(34, 36);
            esp32APSerial.write(mGlobalVariables.ldrState + 40, 0);
            esp32APSerial.write(mGlobalVariables.nightState + 50, 0);
            esp32APSerial.write(mGlobalVariables.displayState + 60, 0);
            esp32APSerial.write(4, Convert.ToInt32(mGlobalVariables.ledSettings));
            esp32APSerial.write(6, Convert.ToInt32(mGlobalVariables.voiceEnabled));
            esp32APSerial.write(mGlobalVariables.tempControl + 70, 0);
            esp32APSerial.write(73, mGlobalVariables.ambientTemperature);
            esp32APSerial.write(74, mGlobalVariables.targetTemperature);
        }

        public void pingBluetooth(int panel)
        {
            SerialOutputWriter serial;
            if(panel == 1)
            {
                serial = esp32APSerial;
                serial.write(2, 0);
            }
        }

        public void voiceSettings(int parameter)
        {
            switch (parameter)
            {
                case OFF:
                    mGlobalVariables.voiceEnabled = false;
                    esp32APSerial.write(6, 0);
                    break;
                case ON:
                    mGlobalVariables.voiceEnabled = true;
                    esp32APSerial.write(6, 1);
                    break;
            }
        }

        public void ledSettings(int parameter)
        {
            switch (parameter)
            {
                case 0:
                    deskArduinoSerial.write(4, 0);
                    break;
                case 1:
                    deskArduinoSerial.write(4, 1);
                    break;
            }
        }

        public void playGreeting()
        {
            Program.mVoiceRecogniton.playSound(@"Greeting.wav");

        }

        public void occupancyWarning()
        {
            Program.mVoiceRecogniton.playSound(@"OccupancyWarning.wav");
            basementArduinoSerial.write(14, 0);
        }

        public void changeOverheadLightState(int state)
        {
            switch (state)
            {
                case OFF:
                    basementArduinoSerial.write(10, 0);
                    break;
                case ON:
                    basementArduinoSerial.write(11, 0);
                    break;
                case TOGGLE:
                    basementArduinoSerial.write(12, 0);
                    break;
            }
        }

        public void dimOverheadLight(int brightness)
        {
            basementArduinoSerial.write(13, brightness);
        }
        
        public void changeFanState(int state)
        {
            switch (state)
            {
                case OFF:
                    basementArduinoSerial.write(20, 0);
                    break;
                case ON:
                    basementArduinoSerial.write(21, 0);
                    break;
                case TOGGLE:
                    basementArduinoSerial.write(22, 0);
                    break;
            }
        }

        public void changeFanSpeed(int speed)
        {
            basementArduinoSerial.write(23, speed);
        }
        
        public void changeTargetTemp(int temperature)
        {
            Debug.WriteLine("Request to change target temp to: " + temperature);
            deskArduinoSerial.write(74, temperature);
        }

        public void updateAmbientTemp()
        {
            deskArduinoSerial.write(73, 0);
        }

        public void changeTempControlState(int state)
        {
            switch (state)
            {
                case OFF:
                    deskArduinoSerial.write(70, 0);
                    break;
                case ON:
                    deskArduinoSerial.write(71, 0);
                    break;
                case TOGGLE:
                    deskArduinoSerial.write(72, 0);
                    break;
            }
        }


        public void changePIRState(int state)
        {
            switch (state)
            {
                case OFF:
                    deskArduinoSerial.write(30, 0);
                    break;
                case ON:
                    deskArduinoSerial.write(31, 0);
                    break;
                case TOGGLE:
                    deskArduinoSerial.write(32, 0);
                    break;
            }
        }

        public void changePIRTimeout(int target, int timeout)
        {
            switch(target)
            {
                case LIGHTS:
                    deskArduinoSerial.write(33, timeout);
                    break;
                case FAN:
                    deskArduinoSerial.write(34, timeout);
                    break;
            }
        }

        public void changeLDRState(int state)
        {
            switch (state)
            {
                case OFF:
                    deskArduinoSerial.write(40, 0);
                    break;
                case ON:
                    deskArduinoSerial.write(41, 0);
                    break;
                case TOGGLE:
                    deskArduinoSerial.write(42, 0);
                    break;
            }
        }

        public void changeNightState(int state)
        {
            switch (state)
            {
                case OFF:
                    mGlobalVariables.nightState = OFF;
                    deskArduinoSerial.write(50, 0);
                    break;
                case ON:
                    mGlobalVariables.nightState = ON;
                    deskArduinoSerial.write(51, 0);
                    break;
                case TOGGLE:
                    if(mGlobalVariables.nightState == ON)
                    {
                        changeNightState(OFF);
                    }
                    else
                    {
                        changeNightState(ON);
                    }
                    break;
            }
            esp32APSerial.write(50 + mGlobalVariables.nightState, 0);
        }

        public void changeDisplayState(int state)
        {
            if (state == OFF)
            {
                Form f = new Form();
                SendMessage(f.Handle, WM_SYSCOMMAND, (IntPtr)SC_MONITORPOWER, (IntPtr)(true ? 2 : -1));
            }
            else
            {
                keybd_event(VK_SHIFT, 0, KEYEVENTF_EXTENTEDKEY, IntPtr.Zero);
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, IntPtr.Zero);
            }
        }

        public void syncOverheadLightState(int state) //Synchronizes all Devices to new Light State
        {
            mGlobalVariables.overheadLightState = state;
            deskArduinoSerial.write(state + 10, 0);
            esp32APSerial.write(state + 10, 0);
            //Console.WriteLine("Lights On");
        }

        public void syncOverheadLightBrightness(int brightness)
        {
            mGlobalVariables.overheadLightBrightness = brightness;
            esp32APSerial.write(13, brightness);
        }

        public void syncFanState(int state) //Synchronizes all Devices to new Fan State
        {
            mGlobalVariables.fanState = state;
            esp32APSerial.write(state + 20, 0);
            deskArduinoSerial.write(state + 20, 0);
        }

        public void syncFanSpeed(int speed)
        {
            mGlobalVariables.fanSpeed = speed;
            esp32APSerial.write(23, speed);
            deskArduinoSerial.write(23, speed);
        }

        public void syncTempControl(int state)
        {
            mGlobalVariables.tempControl = state;
            esp32APSerial.write(state + 70, 0);
        }

        public void syncTargetTemp(int temperature)
        {
            mGlobalVariables.targetTemperature = temperature;
            esp32APSerial.write(74, temperature);
        }

        public void syncAmbientTemp(int temperature)
        {
            mGlobalVariables.ambientTemperature = temperature;
            esp32APSerial.write(73, temperature);
        }

        public void syncPIRState(int state)
        {
            mGlobalVariables.pirState = state;
            esp32APSerial.write(state + 30, 0);
        }

        public void syncPIRTimeout(int target, int timeout)
        {
            switch(target)
            {
                case LIGHTS:
                    esp32APSerial.write(33, timeout);
                    mGlobalVariables.pirLightTimeout = timeout;
                    break;
                case FAN:
                    esp32APSerial.write(34, timeout);
                    mGlobalVariables.pirFanTimeout = timeout;
                    break;
            }
        }

        public void syncLDRState(int state)
        {
            mGlobalVariables.ldrState = state;
            esp32APSerial.write(state + 40, 0);
        }

        public void syncNightModeState(int state)
        {
            esp32APSerial.write(state + 50, 0);
        }

        public void syncDisplayState(int state)
        {
            esp32APSerial.write(state + 60, 0);
        }

        public void syncLEDSettings(int state)
        {
            mGlobalVariables.ledSettings = Convert.ToBoolean(state);
            esp32APSerial.write(4, state);
        }

        public void changeDeskBorderLEDState(int state)
        {
            deskArduinoSerial.write(4, state);
        }
    }
}
