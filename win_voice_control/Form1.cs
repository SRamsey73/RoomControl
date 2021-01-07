using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Room_Control__PC_
{
    public partial class Form1 : Form
    {
        public const int OFF = 0;
        public const int ON = 1;
        public const int TOGGLE = 2;
        const int WM_SYSCOMMAND = 0x112;
        const int SC_MONITORPOWER = 0xF170;

        Form displayForm = new Form();
        VoiceRecognitionHandler mVoiceRecognition = Program.mVoiceRecogniton;
        RoomInteractions mRoomInteractions = Program.mRoomInteractions;
        GlobalVariables mGlobalVariables = Program.mGlobalVariables;
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, IntPtr wParam, IntPtr lParam);
        private void Form1_Load(object sender, EventArgs e)
        {
            if (mGlobalVariables.voiceEnabled == true)
            {
                btnToggleVoice.Text = "Disable Voice";
            }
            else
            {
                btnToggleVoice.Text = "Enable Voice";
            }
            if (Program.deskArduinoSerialPort.IsOpen == true)
            {
                btnDeskPortToggle.Text = "Close Desk Port";
            }
            else
            {
                btnDeskPortToggle.Text = "Open Desk Port";
            }
            if (Program.esp32APSerialPort.IsOpen == true)
            {
                btnBluetoothPortToggle.Text = "Close ESP32 Port";
            }
            else
            {
                btnBluetoothPortToggle.Text = "Open ESP32 Port";
            }
        }

        public Form1()
        {
            InitializeComponent();
        }

        private void btnToggleVoice_Click(object sender, EventArgs e)
        {
            if(mGlobalVariables.voiceEnabled == true)
            {
                mRoomInteractions.voiceSettings(OFF);
                btnToggleVoice.Text = "Enable Voice";
            }
            else
            {
                mRoomInteractions.voiceSettings(ON);
                btnToggleVoice.Text = "Disable Voice";

            }
        }

        private void btnLights_Click(object sender, EventArgs e)
        {
            mRoomInteractions.changeOverheadLightState(2);
        }

        private void btnFan_Click(object sender, EventArgs e)
        {
            mRoomInteractions.changeFanState(TOGGLE);

        }

        private void btnDeskPortToggle_Click(object sender, EventArgs e)
        {
            if(Program.deskArduinoSerialPort.IsOpen == true)
            {
                try
                {
                    Program.deskArduinoSerialPort.Close();
                    btnDeskPortToggle.Text = "Open Desk Port";
                }
                catch (Exception) { }
                
            }
            else
            {
                try
                {
                    Program.deskArduinoSerialPort.Open();
                    btnDeskPortToggle.Text = "Close Desk Port";
                }
                catch (Exception) { }
            }
        }

        private void btnScreenOff_Click(object sender, EventArgs e)
        {
            mRoomInteractions.changeDisplayState(OFF);
        }

        private void btnSync_Click(object sender, EventArgs e)
        {
            mRoomInteractions.syncAll();
        }

        private void printLightState_Click(object sender, EventArgs e)
        {
            Debug.WriteLine(mGlobalVariables.overheadLightState);
        }

        private void btnBluetoothPortToggle_Click(object sender, EventArgs e)
        {
            if (Program.esp32APSerialPort.IsOpen == true)
            {
                try
                {
                    Program.esp32APSerialPort.Close();
                    btnBluetoothPortToggle.Text = "Open ESP32 Port";
                }
                catch (Exception) { }

            }
            else
            {
                try
                {
                    Program.esp32APSerialPort.Open();
                    btnBluetoothPortToggle.Text = "Close Bluetooth Port";
                }
                catch (Exception) { }
            }
        }

        private void txtOverheadLightDimValue_KeyPress(object sender, KeyPressEventArgs e)
        {
            if(e.KeyChar == 13)
            {
                int temp;
                Debug.WriteLine("Pressed");
                if (int.TryParse(txtOverheadLightDimValue.Text, out temp))
                {
                    temp = Convert.ToInt32(txtOverheadLightDimValue.Text);
                    mRoomInteractions.basementArduinoSerial.write(13, temp);
                    Debug.WriteLine(temp);
                    txtOverheadLightDimValue.Text = "";
                }
            }
        }

        private void btnTestVoice_Click(object sender, EventArgs e)
        {
            mVoiceRecognition.testSpeech();
        }
    }
}
