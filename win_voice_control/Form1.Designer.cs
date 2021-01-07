namespace Room_Control__PC_
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnToggleVoice = new System.Windows.Forms.Button();
            this.btnLights = new System.Windows.Forms.Button();
            this.btnFan = new System.Windows.Forms.Button();
            this.btnDeskPortToggle = new System.Windows.Forms.Button();
            this.btnScreenOff = new System.Windows.Forms.Button();
            this.btnSync = new System.Windows.Forms.Button();
            this.printLightState = new System.Windows.Forms.Button();
            this.btnBluetoothPortToggle = new System.Windows.Forms.Button();
            this.txtOverheadLightDimValue = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btnTestVoice = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnToggleVoice
            // 
            this.btnToggleVoice.Location = new System.Drawing.Point(339, 149);
            this.btnToggleVoice.Name = "btnToggleVoice";
            this.btnToggleVoice.Size = new System.Drawing.Size(139, 23);
            this.btnToggleVoice.TabIndex = 0;
            this.btnToggleVoice.Text = "Enable Voice";
            this.btnToggleVoice.UseVisualStyleBackColor = true;
            this.btnToggleVoice.Click += new System.EventHandler(this.btnToggleVoice_Click);
            // 
            // btnLights
            // 
            this.btnLights.Location = new System.Drawing.Point(339, 178);
            this.btnLights.Name = "btnLights";
            this.btnLights.Size = new System.Drawing.Size(139, 23);
            this.btnLights.TabIndex = 1;
            this.btnLights.Text = "Lights";
            this.btnLights.UseVisualStyleBackColor = true;
            this.btnLights.Click += new System.EventHandler(this.btnLights_Click);
            // 
            // btnFan
            // 
            this.btnFan.Location = new System.Drawing.Point(339, 207);
            this.btnFan.Name = "btnFan";
            this.btnFan.Size = new System.Drawing.Size(139, 23);
            this.btnFan.TabIndex = 2;
            this.btnFan.Text = "Fan";
            this.btnFan.UseVisualStyleBackColor = true;
            this.btnFan.Click += new System.EventHandler(this.btnFan_Click);
            // 
            // btnDeskPortToggle
            // 
            this.btnDeskPortToggle.Location = new System.Drawing.Point(339, 346);
            this.btnDeskPortToggle.Name = "btnDeskPortToggle";
            this.btnDeskPortToggle.Size = new System.Drawing.Size(139, 23);
            this.btnDeskPortToggle.TabIndex = 3;
            this.btnDeskPortToggle.Text = "Close Desk Port";
            this.btnDeskPortToggle.UseVisualStyleBackColor = true;
            this.btnDeskPortToggle.Click += new System.EventHandler(this.btnDeskPortToggle_Click);
            // 
            // btnScreenOff
            // 
            this.btnScreenOff.Location = new System.Drawing.Point(339, 317);
            this.btnScreenOff.Name = "btnScreenOff";
            this.btnScreenOff.Size = new System.Drawing.Size(139, 23);
            this.btnScreenOff.TabIndex = 4;
            this.btnScreenOff.Text = "Turn Off Screen";
            this.btnScreenOff.UseVisualStyleBackColor = true;
            this.btnScreenOff.Click += new System.EventHandler(this.btnScreenOff_Click);
            // 
            // btnSync
            // 
            this.btnSync.Location = new System.Drawing.Point(339, 236);
            this.btnSync.Name = "btnSync";
            this.btnSync.Size = new System.Drawing.Size(139, 23);
            this.btnSync.TabIndex = 5;
            this.btnSync.Tag = "";
            this.btnSync.Text = "Sync All";
            this.btnSync.UseVisualStyleBackColor = true;
            this.btnSync.Click += new System.EventHandler(this.btnSync_Click);
            // 
            // printLightState
            // 
            this.printLightState.Location = new System.Drawing.Point(35, 279);
            this.printLightState.Name = "printLightState";
            this.printLightState.Size = new System.Drawing.Size(139, 23);
            this.printLightState.TabIndex = 6;
            this.printLightState.Text = "Print Light State";
            this.printLightState.UseVisualStyleBackColor = true;
            this.printLightState.Click += new System.EventHandler(this.printLightState_Click);
            // 
            // btnBluetoothPortToggle
            // 
            this.btnBluetoothPortToggle.Location = new System.Drawing.Point(339, 375);
            this.btnBluetoothPortToggle.Name = "btnBluetoothPortToggle";
            this.btnBluetoothPortToggle.Size = new System.Drawing.Size(139, 23);
            this.btnBluetoothPortToggle.TabIndex = 7;
            this.btnBluetoothPortToggle.Text = "Close Bluetooth Port";
            this.btnBluetoothPortToggle.UseVisualStyleBackColor = true;
            this.btnBluetoothPortToggle.Click += new System.EventHandler(this.btnBluetoothPortToggle_Click);
            // 
            // txtOverheadLightDimValue
            // 
            this.txtOverheadLightDimValue.Location = new System.Drawing.Point(496, 181);
            this.txtOverheadLightDimValue.Name = "txtOverheadLightDimValue";
            this.txtOverheadLightDimValue.Size = new System.Drawing.Size(100, 20);
            this.txtOverheadLightDimValue.TabIndex = 8;
            this.txtOverheadLightDimValue.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtOverheadLightDimValue_KeyPress);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(518, 165);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 9;
            this.label1.Text = "Dim Lights";
            // 
            // btnTestVoice
            // 
            this.btnTestVoice.Location = new System.Drawing.Point(339, 120);
            this.btnTestVoice.Name = "btnTestVoice";
            this.btnTestVoice.Size = new System.Drawing.Size(139, 23);
            this.btnTestVoice.TabIndex = 10;
            this.btnTestVoice.Text = "Test Voice Assistant";
            this.btnTestVoice.UseVisualStyleBackColor = true;
            this.btnTestVoice.Click += new System.EventHandler(this.btnTestVoice_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.btnTestVoice);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtOverheadLightDimValue);
            this.Controls.Add(this.btnBluetoothPortToggle);
            this.Controls.Add(this.printLightState);
            this.Controls.Add(this.btnSync);
            this.Controls.Add(this.btnScreenOff);
            this.Controls.Add(this.btnDeskPortToggle);
            this.Controls.Add(this.btnFan);
            this.Controls.Add(this.btnLights);
            this.Controls.Add(this.btnToggleVoice);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnToggleVoice;
        private System.Windows.Forms.Button btnLights;
        private System.Windows.Forms.Button btnFan;
        private System.Windows.Forms.Button btnDeskPortToggle;
        private System.Windows.Forms.Button btnScreenOff;
        private System.Windows.Forms.Button btnSync;
        private System.Windows.Forms.Button printLightState;
        private System.Windows.Forms.Button btnBluetoothPortToggle;
        private System.Windows.Forms.TextBox txtOverheadLightDimValue;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnTestVoice;
    }
}

