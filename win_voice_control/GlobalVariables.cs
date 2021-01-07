using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Room_Control__PC_
{
    public class GlobalVariables
    {
        const int OFF = 0;
        const int ON = 1;
        const int TOGGLE = 2;

        public int[] overheadLightDimmerValues = new int[] { 0, 37, 45, 57, 66, 77, 86, 103, 127, 167, 255 };

        public bool voiceEnabled = true;
        public bool ledSettings = true;
        public bool firstSync = false;
        //public int overheadLightState;
        public int overheadLightState;
        public int overheadLightBrightness;
        public int fanState;
        public int fanSpeed;
        public int pirState = ON;
        public int pirLightTimeout = 30;
        public int pirFanTimeout = 30;
        public int tempControl = OFF;
        public int ambientTemperature;
        public int targetTemperature = 75;
        public int ldrState = ON;
        public int nightState = OFF;
        public int displayState = OFF;

    }
}
