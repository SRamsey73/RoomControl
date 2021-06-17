using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Speech.Synthesis;
using System.Speech.Recognition;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Speech.AudioFormat;
using static Room_Control__PC_.RoomInteractions;

namespace Room_Control__PC_
{
    public class VoiceRecognitionHandler
    {
        RoomInteractions mRoomInteractions = Program.mRoomInteractions;
        GlobalVariables mGlobalVariables = Program.mGlobalVariables;
        //Constants
        //Listening Constants
        const int LWAKEWORD = 0;
        const int LCOMMAND = 1;
        const int LFANSPEED = 2;
        const int LDIMLIGHTS = 3;
        const String WAKEWORD = "Computer";
        const int OFF = 0;
        const int ON = 1;
        const int TOGGLE = 2;
        //Sound Variables
        const String SOUNDLOCATION = @"I:\Projects\Room_Control\win_voice_control\Sounds\";
        const String LISTENINGSOUND = @"Listening.wav";
        const String LIGHTSOUND = @"LightNoise.wav";
        const String CANCELSOUND = @"CancelNoise2.wav";
        const String PLAYSOUND = @"Play.wav";
        const String PAUSESOUND = @"Pause.wav";
        const String CHANGETRACKSOUND = @"ChangeTrack.wav";
        const String NIGHTPROTOCOLACTIVATED = @"NightProtocolActivate.wav";
        const String DISPLAY = @"Display.wav";
        const String CONFIRMINPUT = @"ConfirmInput.wav";
        const String LIGHTSDIMMED = @"LightsDimmed.wav";
        const String FANSPEED = @"FanSpeed.wav";
        const String CANCEL = @"Cancel.wav";
        //Key Definitions
        public const int KEYEVENTF_EXTENTEDKEY = 1;
        public const int KEYEVENTF_KEYUP = 0;
        public const int VK_MEDIA_NEXT_TRACK = 0xB0; // code to jump to next track
        public const int VK_MEDIA_PLAY_PAUSE = 0xB3; // code to play or pause a song
        public const int VK_MEDIA_PREV_TRACK = 0xB1; // code to jump to prev track
        public const int VK_SHIFT = 0x10;
        [DllImport("user32.dll")]
        public static extern void keybd_event(byte virtualKey, byte scanCode, uint flags, IntPtr extraInfo);

        //Listening State Variables
        int listeningState;
        //public bool voiceEnabled = true;
        //Speech Recognition and Synthesizer Constructors
        public SpeechRecognitionEngine recEngine = new SpeechRecognitionEngine(new CultureInfo("en-US"));
        public SpeechSynthesizer virtualAssistant = new SpeechSynthesizer();
        GrammarBuilder wakeWord = new GrammarBuilder();
        GrammarBuilder commandKeywords = new GrammarBuilder();
        GrammarBuilder fanSpeedKeywords = new GrammarBuilder();
        GrammarBuilder lightLevelKeywords = new GrammarBuilder();
        Choices commandDefinitions = new Choices();
        Choices fanSpeedDefinitions = new Choices();
        Choices lightLevelDefinitiions = new Choices();
        Grammar commandGrammar;
        Grammar fanSpeedGrammar;
        Grammar lightLevelGrammar;
        Grammar wakeWordGrammar;
        DictationGrammar mDictationGrammar;
        string[] lightLevelStrings = new string[14] { "minimum", "ten percent", "twenty percent", "thirty perecent", "fourty percent", "fifty percent", "sixty percent",
                "seventy percent", "eighty percent", "ninety percent", "one hundered percent", "full", "maximum", "cancel"};
    //Sound Player
    System.Media.SoundPlayer soundPlayer = new System.Media.SoundPlayer();

        //Main entry point
        public VoiceRecognitionHandler()
        {
            Debug.WriteLine("Voice Recognition Started");
            soundPlayer.SoundLocation = @"I:\Projects\Room\Software\PC\Room Control (PC)\Room Control (PC)\Sounds\Listening.wav";
            //soundPlayer.Play();
            //Voice Command Definitions
            //Commands
            commandDefinitions.Add(new string[] {"lights", "time", "date", "fan", "set fan speed", "cancel", "resume", "pause", "previous track", "next track",
                "activate night mode", "deactivate night mode", "activate display", "deactivate display", "disable motion sensor", "enable motion sensor",
                "disable ambient light sensor", "enable ambient light sensor", "set light level", "night mode", "sleep", "show console", "hide console"});
            commandKeywords.Append(commandDefinitions);
            commandGrammar = new Grammar(commandDefinitions);
            //recEngine.LoadGrammar(commandGrammar);

            //Fan speed Commands
            fanSpeedDefinitions.Add(new string[] { "low speed", "medium", "high speed", "cancel" });
            fanSpeedKeywords.Append(fanSpeedDefinitions);
            fanSpeedGrammar = new Grammar(fanSpeedDefinitions);
            //recEngine.LoadGrammar(fanSpeedGrammar);

            //Dim lights Commands
            lightLevelDefinitiions.Add(lightLevelStrings);
            lightLevelKeywords.Append(lightLevelDefinitiions);
            lightLevelGrammar = new Grammar(lightLevelKeywords);
            //Listening only for computer
            wakeWord.Append(WAKEWORD);
            wakeWordGrammar = new Grammar(wakeWord);

            configureRecEngine();

            //Sound Player Settings

            //Virtual Assistant Settings
            //printInstalledVoices();
            virtualAssistant.SelectVoice("Microsoft Zira Desktop");
            virtualAssistant.Volume = 100;

            testSpeech();
        }

        public void configureRecEngine()
        {
            if (this.recEngine != null)
            {
                this.recEngine.Dispose();
            }
            this.recEngine = new SpeechRecognitionEngine(new CultureInfo("en-US"));
            //Speech Recognition Settings 
            recEngine.LoadGrammar(wakeWordGrammar);
            mDictationGrammar = new DictationGrammar("grammar:dictation#pronunciation");
            mDictationGrammar.Name = "Random";
            recEngine.LoadGrammar(mDictationGrammar);
            recEngine.MaxAlternates = 1;
            //recEngine.BabbleTimeout = TimeSpan.FromSeconds(5);
            recEngine.SetInputToDefaultAudioDevice();
            recEngine.RecognizeAsync(RecognizeMode.Multiple);
            recEngine.SpeechRecognized += RecEngine_SpeechRecognized;
        }

        //Speech Engine Recognized Speech
        public void RecEngine_SpeechRecognized(object sender, SpeechRecognizedEventArgs e)
        {
            Console.WriteLine("Recognized: " + e.Result.Text + " | Confidence: " + e.Result.Confidence + " | Listening For: " + listeningState);
            if (mGlobalVariables.voiceEnabled && e.Result.Grammar.Name != "Random") 
            {
                if (listeningState == LWAKEWORD && e.Result.Text == WAKEWORD && e.Result.Confidence > .5)
                {
                    mRoomInteractions.changeDeskBorderLEDState(26);
                    playSound(LISTENINGSOUND);
                    listeningState = LCOMMAND;
                    recEngine.UnloadGrammar(mDictationGrammar);
                    recEngine.UnloadGrammar(wakeWordGrammar);
                    recEngine.LoadGrammar(commandGrammar);
                }
                else if (listeningState == LCOMMAND && e.Result.Confidence > .94 && e.Result.Grammar == commandGrammar)
                {
                    listeningState = LWAKEWORD;
                    mRoomInteractions.changeDeskBorderLEDState(27);
                    switch (e.Result.Text)
                    {
                        case "lights":
                            mRoomInteractions.changeOverheadLightState(State.TOGGLE);
                            playSoundSync(LIGHTSOUND);
                            Thread.Sleep(500);
                            break;
                        case "time":
                            string[] timeString = (((DateTime.Now).ToString()).Split(' '));
                            string[] timeString2 = timeString[1].Split(':');
                            virtualAssistant.Speak("The current time is " + timeString2[0] + ":" + timeString2[1]);
                            break;
                        case "date":
                            string[] dateString = (((DateTime.Now).ToString()).Split(' '));
                            virtualAssistant.Speak("The date is " + dateString[0]);

                            break;
                        case "fan":
                            mRoomInteractions.changeFanState(State.TOGGLE);
                            playSound(PLAYSOUND);
                            break;
                        case "set fan speed":
                            listeningState = LFANSPEED;
                            virtualAssistant.Speak("state desired speed");
                            recEngine.LoadGrammar(fanSpeedGrammar);
                            mRoomInteractions.changeDeskBorderLEDState(26);
                            break;
                        case "cancel":
                            playSoundSync(CANCEL);
                            //virtualAssistant.Speak("Command input canceled");
                            break;
                        case "resume":
                            playSoundSync(PLAYSOUND);
                            keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_EXTENTEDKEY, IntPtr.Zero);
                            break;
                        case "pause":
                            keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_EXTENTEDKEY, IntPtr.Zero);
                            playSoundSync(PAUSESOUND);
                            break;
                        case "previous track":
                            keybd_event(VK_MEDIA_PREV_TRACK, 0, KEYEVENTF_EXTENTEDKEY, IntPtr.Zero);
                            keybd_event(VK_MEDIA_PREV_TRACK, 0, KEYEVENTF_EXTENTEDKEY, IntPtr.Zero);
                            playSoundSync(CHANGETRACKSOUND);
                            break;
                        case "next track":
                            keybd_event(VK_MEDIA_NEXT_TRACK, 0, KEYEVENTF_EXTENTEDKEY, IntPtr.Zero);
                            playSoundSync(CHANGETRACKSOUND);
                            break;
                        case "deactivate night mode":
                            playSound(NIGHTPROTOCOLACTIVATED);
                            mRoomInteractions.changeNightState(State.OFF);
                            //virtualAssistant.Speak("Night protocol deactivated");
                            break;
                        case "activate night mode":
                            playSound(NIGHTPROTOCOLACTIVATED);
                            mRoomInteractions.changeNightState(State.ON);

                            //virtualAssistant.Speak("Night protocol activated");
                            break;
                        case "night mode":
                            mRoomInteractions.changeNightState(State.TOGGLE);
                            playSound(NIGHTPROTOCOLACTIVATED);
                            break;
                        case "deactivate display":
                            mRoomInteractions.changeDisplayState(State.OFF);
                            playSoundSync(DISPLAY);
                            break;
                        case "activate display":
                            mRoomInteractions.changeDisplayState(State.ON);
                            virtualAssistant.Speak("Display activated");
                            break;
                        case "disable motion sensor":
                            mRoomInteractions.changePIRState(OFF);
                            playSoundSync(CONFIRMINPUT);
                            //virtualAssistant.Speak("motion sensor disabled");
                            break;
                        case "enable motion sensor":
                            mRoomInteractions.changePIRState(ON);
                            playSoundSync(CONFIRMINPUT);
                            //virtualAssistant.Speak("motion sensor enabled");
                            break;
                        case "disable ambient light sensor":
                            mRoomInteractions.changeLDRState(OFF);
                            playSoundSync(CONFIRMINPUT);
                            //virtualAssistant.Speak("ambient light sensor disabled");
                            break;
                        case "enable ambient light sensor":
                            mRoomInteractions.changeLDRState(ON);
                            playSoundSync(CONFIRMINPUT);
                            //virtualAssistant.Speak("ambient light sensor enabled");
                            break;
                        case "set light level":
                            listeningState = LDIMLIGHTS;
                            virtualAssistant.Speak("state desired brightness");
                            mRoomInteractions.changeDeskBorderLEDState(26);
                            recEngine.LoadGrammar(lightLevelGrammar);
                            break;
                        case "sleep":
                            playSoundSync(LIGHTSOUND);
                            mRoomInteractions.sleepPC();
                            break;
                        case "show console":
                            Program.ShowWindow(Program.console, Program.SW_SHOW);
                            playSound(PLAYSOUND);
                            break;
                        case "hide console":
                            Program.ShowWindow(Program.console, Program.SW_HIDE);
                            playSound(PAUSESOUND);
                            break;
                    }
                    if (listeningState == LWAKEWORD)
                    {
                        recEngine.LoadGrammarAsync(mDictationGrammar);
                        mRoomInteractions.changeDeskBorderLEDState(28);
                        recEngine.LoadGrammar(wakeWordGrammar);
                    }
                    recEngine.UnloadGrammar(commandGrammar);
                }
                else if (listeningState == LFANSPEED && e.Result.Confidence > .9 && e.Result.Grammar == fanSpeedGrammar)
                {
                    mRoomInteractions.changeDeskBorderLEDState(27);
                    playSoundSync(FANSPEED);
                    switch (e.Result.Text)
                    {
                        case "low speed":
                            mRoomInteractions.changeFanSpeed(FanSpeed.LOW);
                            //virtualAssistant.Speak("Fan speed set to low");
                            break;
                        case "medium":
                            mRoomInteractions.changeFanSpeed(FanSpeed.MEDIUM);
                            //virtualAssistant.Speak("Fan speed set to medium");
                            break;
                        case "high speed":
                            mRoomInteractions.changeFanSpeed(FanSpeed.HIGH);
                            //virtualAssistant.Speak("Fan speed set to high");
                            break;
                        case "cancel":
                            playSound(CANCEL);
                            break;
                    }
                    listeningState = LWAKEWORD;
                    mRoomInteractions.changeDeskBorderLEDState(28);
                    recEngine.UnloadGrammar(fanSpeedGrammar);
                    recEngine.LoadGrammarAsync(mDictationGrammar);
                    recEngine.LoadGrammar(wakeWordGrammar);
                }
                else if(listeningState == LDIMLIGHTS && e.Result.Confidence > .9 && e.Result.Grammar == lightLevelGrammar)
                {
                    mRoomInteractions.changeDeskBorderLEDState(27);
                    for (int i = 0; i < 14; i++)
                    {
                        if (e.Result.Text == lightLevelStrings[i])
                        {
                            if(i == 0)
                            {
                                i = 1;
                            }
                            else if(i == 13)
                            {
                                playSound(CANCEL);
                                break;
                            }
                            else if(i > 10)
                            {
                                i = 10;
                            }
                            mRoomInteractions.changeOverheadLightBrightness(i*10);
                            playSound(LIGHTSDIMMED);
                            //virtualAssistant.Speak("light level set to " + e.Result.Text);
                            break;
                        }
                    }
                    listeningState = LWAKEWORD;
                    mRoomInteractions.changeDeskBorderLEDState(28);
                    recEngine.UnloadGrammar(lightLevelGrammar);
                    recEngine.LoadGrammarAsync(mDictationGrammar);
                    recEngine.LoadGrammar(wakeWordGrammar);
                }
                    /*if (listeningState == LWAKEWORD)
                     {
                         listeningState = LCOMMAND;
                         recEngine.UnloadAllGrammars();
                         recEngine.LoadGrammarAsync(commandGrammar);
                     }
                     else if (listeningState == LCOMMAND)
                     {
                         listeningState = LWAKEWORD;
                         recEngine.UnloadAllGrammars();
                         recEngine.LoadGrammarAsync(wakeWordGrammar);
                     }*/
                }
        }
        public void playSound(string sound)
        {
            soundPlayer.SoundLocation = SOUNDLOCATION + sound;
            soundPlayer.Play();
        }
        private void playSoundSync(string sound)
        {
            soundPlayer.SoundLocation = SOUNDLOCATION + sound;
            soundPlayer.PlaySync();
        }

        public void testSpeech()
        {
            virtualAssistant.Speak("Voice recognition service initialized");
        }

        public void printInstalledVoices()
        {
            using (SpeechSynthesizer synth = new SpeechSynthesizer())
            {

                // Output information about all of the installed voices.   
                Console.WriteLine("Installed voices -");
                foreach (InstalledVoice voice in synth.GetInstalledVoices())
                {
                    VoiceInfo info = voice.VoiceInfo;
                    string AudioFormats = "";
                    foreach (SpeechAudioFormatInfo fmt in info.SupportedAudioFormats)
                    {
                        AudioFormats += String.Format("{0}\n",
                        fmt.EncodingFormat.ToString());
                    }

                    Console.WriteLine(" Name:          " + info.Name);
                    Console.WriteLine(" Culture:       " + info.Culture);
                    Console.WriteLine(" Age:           " + info.Age);
                    Console.WriteLine(" Gender:        " + info.Gender);
                    Console.WriteLine(" Description:   " + info.Description);
                    Console.WriteLine(" ID:            " + info.Id);
                    Console.WriteLine(" Enabled:       " + voice.Enabled);
                    if (info.SupportedAudioFormats.Count != 0)
                    {
                        Console.WriteLine(" Audio formats: " + AudioFormats);
                    }
                    else
                    {
                        Console.WriteLine(" No supported audio formats found");
                    }

                    string AdditionalInfo = "";
                    foreach (string key in info.AdditionalInfo.Keys)
                    {
                        AdditionalInfo += String.Format("  {0}: {1}\n", key, info.AdditionalInfo[key]);
                    }

                    Console.WriteLine(" Additional Info - " + AdditionalInfo);
                    Console.WriteLine();
                }
            }
        }
    }
}
