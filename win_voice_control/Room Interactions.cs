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

        public enum State {
            ON,
            OFF,
            TOGGLE
        }

        public enum FanSpeed
        {
            LOW,
            MEDIUM,
            HIGH
        }

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

        public RoomInteractions()
        {
            Debug.WriteLine("RoomInteractions");
        }

        private static string stateToString(State state) {
            string result;
            switch (state)
            {
                case State.OFF:
                    result = "off";
                    break;
                case State.ON:
                    result = "on";
                    break;
                case State.TOGGLE:
                    result = "toggle";
                    break;
                default:
                    result = "undefined";
                    break;
            }
            return result;
        }

        private static string fanSpeedToString(FanSpeed speed)
        {
            string result;
            switch (speed)
            {
                case FanSpeed.LOW:
                    result = "low";
                    break;
                case FanSpeed.MEDIUM:
                    result = "medium";
                    break;
                case FanSpeed.HIGH:
                    result = "high";
                    break;
                default:
                    result = "undefined";
                    break;
            }
            return result;
        }

        public void changeOverheadLightState(State state)
        {
            Program.roomPi.send("overhead_light:state:" + stateToString(state));
        }

        public void changeOverheadLightBrightness(int brightness)
        {
            Program.roomPi.send("overhead_light:brightness:" + brightness);
        }
        
        public void changeFanState(State state)
        {
            Program.roomPi.send("ceiling_fan:state:" + stateToString(state));
        }

        public void changeFanSpeed(FanSpeed speed)
        {
            Program.roomPi.send("ceiling_fan:speed:" + fanSpeedToString(speed));
        }
        
        public void changeTargetTemp(int temperature)
        {
        }

        public void updateAmbientTemp()
        {
        }

        public void changeTempControlState(int state)
        {

        }


        public void changePIRState(int state)
        {

        }

        public void changePIRTimeout(int target, int timeout)
        {

        }

        public void changeLDRState(int state)
        {

        }

        public void changeNightState(State state)
        {
            Program.roomPi.send("night_mode:state:" + stateToString(state));
        }

        public void changeDisplayState(State state)
        {
            if (state == State.OFF)
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

        public void changeDeskBorderLEDState(int state)
        {
        }

        public void sleepPC()
        {
            Application.SetSuspendState(PowerState.Suspend, false, false);
        }
    }
}
