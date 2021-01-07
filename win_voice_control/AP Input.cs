using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Room_Control__PC_
{
    class AP_Input
    {
        RoomInteractions mRoomInteractions = Program.mRoomInteractions;
        GlobalVariables mGlobalVariables = Program.mGlobalVariables;

        const int OFF = 0;
        const int ON = 1;
        const int TOGGLE = 2;
        const int LIGHTS = 0;
        const int FAN = 1;
    }
}
