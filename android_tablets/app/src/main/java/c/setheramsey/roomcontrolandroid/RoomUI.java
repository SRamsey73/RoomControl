package c.setheramsey.roomcontrolandroid;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.TransitionDrawable;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.support.constraint.ConstraintLayout;
import android.support.v4.content.res.ResourcesCompat;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.DecelerateInterpolator;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextClock;
import android.widget.TextView;

import java.util.Timer;
import java.util.TimerTask;


public class RoomUI extends Activity {
    //CONSTANTS=====================================================================================
    String TAG = "RoomUI";
    final int OFF = 0;
    final int ON = 1;
    final int TOGGLE = 2;
    final int OVERHEAD_LIGHTS = 0;
    final int FAN = 1;
    final int[] overheadLightDimmerValues = new int[]{0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    //VARIABLES=====================================================================================
    public Context context;
    private int wakeLockState = 1;
    //Panels
    View currentPanel;
    //Light Panel
    int lightLevel = 5;
    int lightODSTimeout;
    //Fan Panel
    int fanODSTimeout;
    int targetTemp = 0;
    public int ambientTemp = 0;
    //Auxiliary
    public int deskLEDState;
    public int voiceState;
    ConstraintLayout nightClockLayout;
    //Views=========================================================================================
    //Header Block
    public TextClock txtClock;
    public TextClock txtDay;
    public TextClock txtMonth;
    public TextView txtAmbientTemp;
    public TextView txtSevenSegmentBackground;
    public TextView txtYearBackground;
    //Light Block
    public TextView txtLights;
    public TextView txtPIR_lights;
    public TextView txtLDR_lights;
    public ImageView imgLightsIndicator;
    public ImageView imgPIRIndicator_lights;
    public ImageView imgLDRIndicator_lights;
    public ImageButton btnLights;
    public ImageButton btnPIR_lights;
    public ImageButton btnLDR_lights;
    //Light Panel
    public View pnlLights;
    public TextView txtLightConfigPanel;
    public TextView txtIntensity_lp;
    public TextView txtODSTimeout_lp;
    public TextView txtALSThreshold_lp;
    public TextView txtODSValue_lp;
    public TextView txtALSValue_lp;
    public ImageView[] img_light_bars = new ImageView[11];
    public ImageButton btnODSIncrease_lp;
    public ImageButton btnODSDecrease_lp;
    public ImageButton btnALSIncrease_lp;
    public ImageButton btnALSDecrease_lp;
    public ImageButton btnDecreaseLight;
    public ImageButton btnIncreaseLight;
    //Fan Block
    public TextView txtFan;
    public TextView txtPIR_fan;
    public TextView txtTemp_fan;
    public ImageView imgFanIndicator;
    public ImageView imgPIRIndicator_fan;
    public ImageView imgTempIndicator_fan;
    public ImageButton btnFan;
    public ImageButton btnPIR_fan;
    public ImageButton btnTEMP_fan;
    //Fan Panel
    public View pnlFan;
    public TextView txtFanConfigPanel;
    public TextView txtSpeed_fp;
    public TextView txtODSTimeout_fp;
    public TextView txtTemperature_fp;
    public TextView txtODSValue_fp;
    public TextView txtODSLabel_fp;
    public TextView txtTempValue_fp;
    public TextView txtTempLabel_fp;
    public TextView txtLowFanSpeed;
    public TextView txtMediumFanSpeed;
    public TextView txtHighFanSpeed;
    public ImageView imgTempControlIndicator;
    public ImageView imgLowFanSpeedIndicator;
    public ImageView imgMediumFanSpeedIndicator;
    public ImageView imgHighFanSpeedIndicator;
    public ImageButton btnLowFanSpeed;
    public ImageButton btnMediumFanSpeed;
    public ImageButton btnHighFanSpeed;
    public ImageButton btnIncreaseODS_fp;
    public ImageButton btnDecreaseODS_fp;
    public ImageButton btnIncreaseTemp_fp;
    public ImageButton btnDecreaseTemp_fp;
    //Auxillary Views
    TextView txtNight;
    TextView txtNightClockLabel;
    TextView txtDeskLEDs;
    TextView txtSleepPC;
    TextView txtWakePC;
    TextView txtVoice;
    public ImageView imgNightIndicator;
    public ImageView imgDeskLEDIndicator;
    public ImageView imgVoiceIndicator;
    ImageButton btnNight;
    ImageButton btnNightClock;
    ImageButton btnDeskLEDs;
    ImageButton btnSleepPC;
    ImageButton btnWakePC;
    ImageButton btnVoice;
    //Connection Panel
    View pnlConnectionLost;
    TextView txtConnectionLost;
    TextView txtReconnect;
    ImageView imgConnectionPanelBrackets;
    ImageButton btnReconnectBluetooth;
    //Misc Views
    //Fonts=========================================================================================
    Typeface tfSevenSegment;
    Typeface tfControlFreak;
    Typeface tfAovLight;
    Typeface tfNasa;
    //Animations====================================================================================
    public Animation panelFadeIn = new AlphaAnimation(0f, 1f);
    public Animation panelFadeOut = new AlphaAnimation(1f, 0f);
    private Animation indicatorFadeIn = new AlphaAnimation(0f, 1f);
    private Animation indicatorFadeOut = new AlphaAnimation(1f, 0f);
    private Drawable[] buttonDrawables = new Drawable[2];
    private Drawable[] lightBarDrawables = new Drawable[2];
    private TransitionDrawable[] buttonClickFader = new TransitionDrawable[10];
    private TransitionDrawable[] lightBarFader = new TransitionDrawable[11];
    private int[] lightBarStates = new int[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    private int[] currentFader = new int[]{0, 0};
    private int[] delayedFader = new int[]{0, 0};
    private MediaPlayer mediaPlayer = new MediaPlayer();
    //Objects=======================================================================================
    //private BluetoothCommunication bluetoothCommunication;
    private WiFiCommunicationPi wifiCommunication;

    //SETUP METHODS=================================================================================
    //Configure
    void configureObjects() {
        tfSevenSegment = ResourcesCompat.getFont(getApplicationContext(), R.font.seven_segment);
        tfControlFreak = ResourcesCompat.getFont(getApplicationContext(), R.font.control_freak);
        tfAovLight = ResourcesCompat.getFont(getApplicationContext(), R.font.aov_light);
        tfNasa = ResourcesCompat.getFont(getApplicationContext(), R.font.nasalization);

        //bluetoothCommunication = new BluetoothCommunication(this);
        wifiCommunication = new WiFiCommunicationPi(this);
    }
    void configureFadeProperties() {
        panelFadeOut.setInterpolator(new AccelerateInterpolator());
        panelFadeOut.setDuration(725);
        panelFadeIn.setInterpolator(new DecelerateInterpolator());
        panelFadeIn.setDuration(100);
        indicatorFadeOut.setInterpolator(new AccelerateInterpolator());
        indicatorFadeOut.setDuration(400);
        indicatorFadeOut.setInterpolator(new DecelerateInterpolator());
        indicatorFadeIn.setDuration(400);

        lightBarDrawables[0] = getDrawable(R.drawable.ic_light_bar_empty);
        lightBarDrawables[1] = getDrawable(R.drawable.ic_light_bar_full);
        for(int i = 1; i <= 10; i++) {
            lightBarFader[i] = new TransitionDrawable(lightBarDrawables);
            img_light_bars[i].setImageDrawable(lightBarFader[i]);
        }
    }
    //Decor View
    void setDecorView() {
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_USER_LANDSCAPE);
        final View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        decorView.setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
            @Override
            public void onSystemUiVisibilityChange(int visibility) {
                if (visibility != View.SYSTEM_UI_FLAG_FULLSCREEN) {
                    new Timer().schedule(new TimerTask() {
                        @Override
                        public void run() {
                            Utils.runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
                                }
                            });
                        }
                    },3000);
                }
            }
        });
    }
    //Find Views
    void setHeaderViews() {
        txtClock = findViewById(R.id.txtClock);
        txtMonth = findViewById(R.id.txtMonth);
        txtDay = findViewById(R.id.txtDay);
        txtAmbientTemp = findViewById(R.id.txtAmbientTemp);

        txtClock.setTypeface(tfAovLight);
        txtMonth.setTypeface(tfAovLight);
        txtDay.setTypeface(tfAovLight);
        txtAmbientTemp.setTypeface(tfAovLight);
    }
    void setLightBlockViews() {
        txtLights = findViewById(R.id.txtLights);
        txtPIR_lights = findViewById(R.id.txtPIR_lights);
        txtLDR_lights = findViewById(R.id.txtLDR_lights);
        imgLightsIndicator = findViewById(R.id.imgLightsIndicator);
        imgPIRIndicator_lights = findViewById(R.id.imgPIRIndicator_lights);
        imgLDRIndicator_lights = findViewById(R.id.imgLDRIndicator_lights);
        btnLights = findViewById(R.id.btnLights);
        btnPIR_lights = findViewById(R.id.btnPIR_lights);
        btnLDR_lights = findViewById(R.id.btnLDR_lights);

        txtLights.setTypeface(tfControlFreak);
        txtPIR_lights.setTypeface(tfControlFreak);
        txtLDR_lights.setTypeface(tfControlFreak);
    }
    void setLightPanelViews() {
        pnlLights = findViewById(R.id.pnlLights);
        txtLightConfigPanel = findViewById(R.id.txtLightConfigPanel);
        txtIntensity_lp = findViewById(R.id.txtIntensity_lp);
        txtODSTimeout_lp = findViewById(R.id.txtODSTimeout_lp);
        txtALSThreshold_lp = findViewById(R.id.txtALSThreshold_lp);
        txtODSValue_lp = findViewById(R.id.txtODSValue_lp);
        txtALSValue_lp = findViewById(R.id.txtALSValue_lp);
        img_light_bars[1] = findViewById(R.id.light_bar_1);
        img_light_bars[2] = findViewById(R.id.light_bar_2);
        img_light_bars[3] = findViewById(R.id.light_bar_3);
        img_light_bars[4] = findViewById(R.id.light_bar_4);
        img_light_bars[5] = findViewById(R.id.light_bar_5);
        img_light_bars[6] = findViewById(R.id.light_bar_6);
        img_light_bars[7] = findViewById(R.id.light_bar_7);
        img_light_bars[8] = findViewById(R.id.light_bar_8);
        img_light_bars[9] = findViewById(R.id.light_bar_9);
        img_light_bars[10] = findViewById(R.id.light_bar_10);
        btnDecreaseLight = findViewById(R.id.btnDecreaseLight);
        btnIncreaseLight = findViewById(R.id.btnIncreaseLight);
        btnODSIncrease_lp = findViewById(R.id.btnIncreaseODS_lp);
        btnODSDecrease_lp = findViewById(R.id.btnDecreaseODS_lp);
        btnALSIncrease_lp = findViewById(R.id.btnIncreaseALS_lp);
        btnALSDecrease_lp = findViewById(R.id.btnDecreaseALS_lp);

        txtLightConfigPanel.setTypeface(tfControlFreak);
        txtIntensity_lp.setTypeface(tfControlFreak);
        txtODSTimeout_lp.setTypeface(tfControlFreak);
        txtALSThreshold_lp.setTypeface(tfControlFreak);
        txtODSValue_lp.setTypeface(tfNasa);
        txtALSValue_lp.setTypeface(tfNasa);
    }
    void setFanBlockViews() {
        txtFan = findViewById(R.id.txtFAN);
        txtPIR_fan = findViewById(R.id.txtPIR_fan);
        txtTemp_fan = findViewById(R.id.txtTEMP);
        imgFanIndicator = findViewById(R.id.imgFanIndicator);
        imgPIRIndicator_fan = findViewById(R.id.imgPIRIndicator_fan);
        imgTempIndicator_fan = findViewById(R.id.imgTempIndicator_fan);
        btnFan = findViewById(R.id.btnFan);
        btnPIR_fan = findViewById(R.id.btnPIR_fan);
        btnTEMP_fan = findViewById(R.id.btnTEMP_fan);

        txtFan.setTypeface(tfControlFreak);
        txtPIR_fan.setTypeface(tfControlFreak);
        txtTemp_fan.setTypeface(tfControlFreak);
    }
    void setFanPanelViews() {
        pnlFan = findViewById(R.id.pnlFan);
        pnlFan.setVisibility(View.GONE);
        txtLowFanSpeed = findViewById(R.id.txtLowFanSpeed);
        txtMediumFanSpeed = findViewById(R.id.txtMediumFanSpeed);
        txtHighFanSpeed = findViewById(R.id.txtHighFanSpeed);
        txtFanConfigPanel = findViewById(R.id.txtFanConfigPanel);
        txtSpeed_fp = findViewById(R.id.txtSpeed_fp);
        txtODSTimeout_fp = findViewById(R.id.txtODSTimeout_fp);
        txtODSValue_fp = findViewById(R.id.txtODSValue_fp);
        txtTemperature_fp = findViewById(R.id.txtTemperature_fp);
        txtTempValue_fp = findViewById(R.id.txtTempValue_fp);
        imgLowFanSpeedIndicator = findViewById(R.id.imgLowFanSpeedIndicator);
        imgMediumFanSpeedIndicator = findViewById(R.id.imgMediumFanSpeedIndicator);
        imgHighFanSpeedIndicator = findViewById(R.id.imgHighFanSpeedIndicator);
        imgTempControlIndicator = findViewById(R.id.imgTempIndicator_fan);
        btnLowFanSpeed = findViewById(R.id.btnLowFanSpeed);
        btnMediumFanSpeed = findViewById(R.id.btnMediumFanSpeed);
        btnHighFanSpeed = findViewById(R.id.btnHighFanSpeed);
        btnIncreaseODS_fp = findViewById(R.id.btnIncreaseODS_fp);
        btnDecreaseODS_fp = findViewById(R.id.btnDecreaseODS_fp);
        btnIncreaseTemp_fp = findViewById(R.id.btnIncreaseTemp_fp);
        btnDecreaseTemp_fp = findViewById(R.id.btnDecreaseTemp_fp);
        btnTEMP_fan = findViewById(R.id.btnTEMP_fan);

        txtLowFanSpeed.setTypeface(tfControlFreak);
        txtMediumFanSpeed.setTypeface(tfControlFreak);
        txtHighFanSpeed.setTypeface(tfControlFreak);
        txtFanConfigPanel.setTypeface(tfControlFreak);
        txtSpeed_fp.setTypeface(tfControlFreak);
        txtODSTimeout_fp.setTypeface(tfControlFreak);
        txtODSValue_fp.setTypeface(tfNasa);
        txtTemperature_fp.setTypeface(tfControlFreak);
        txtTempValue_fp.setTypeface(tfNasa);
    }
    void setAuxiliaryViews() {
        txtNight = findViewById(R.id.txtNight);
        txtNightClockLabel = findViewById(R.id.txtNightClockLabel);
        txtDeskLEDs = findViewById(R.id.txtDeskLEDs);
        txtVoice = findViewById(R.id.txtVoice);
        txtSleepPC = findViewById(R.id.txtSleepPC);
        txtWakePC = findViewById(R.id.txtWakePC);

        imgNightIndicator = findViewById(R.id.imgNightIndicator);
        imgDeskLEDIndicator = findViewById(R.id.imgDeskLEDIndicator);
        imgVoiceIndicator = findViewById(R.id.imgVoiceIndicator);

        btnNight = findViewById(R.id.btnNightProtocol);
        btnNightClock = findViewById(R.id.btnNightClock);
        btnDeskLEDs = findViewById(R.id.btnDeskLEDs);
        btnSleepPC = findViewById(R.id.btnSleepPC);
        btnWakePC = findViewById(R.id.btnWakePC);
        btnVoice = findViewById(R.id.btnVoice);

        txtNight.setTypeface(tfControlFreak);
        txtDeskLEDs.setTypeface(tfControlFreak);
        txtVoice.setTypeface(tfControlFreak);
        txtSleepPC.setTypeface(tfControlFreak);
        txtWakePC.setTypeface(tfControlFreak);
        txtNightClockLabel.setTypeface(tfControlFreak);
        nightClockLayout = findViewById(R.id.nightClockLayout);
    }
    void setConnectionPanelViews() {
        pnlConnectionLost = findViewById(R.id.pnlConnectionLost);
        txtConnectionLost = findViewById(R.id.txtConectionLost);
        txtReconnect = findViewById(R.id.txtReconnect);
        imgConnectionPanelBrackets = findViewById(R.id.imgConnectionBrackets);
        btnReconnectBluetooth = findViewById(R.id.btnReconnectBluetooth);

        txtConnectionLost.setTypeface(tfControlFreak);
        txtReconnect.setTypeface(tfControlFreak);
    }
    void setMiscViews(){
    }
    void setStartingPanel(View panel) {
        pnlLights.setVisibility(View.GONE);
        pnlFan.setVisibility(View.GONE);
        pnlConnectionLost.setVisibility(View.GONE);
        panel.setVisibility(View.VISIBLE);
        currentPanel = panel;
    }
    //On Click Listeners
    void setLightBlockClickListeners() {
        btnLights.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                wifiCommunication.send(RoomProperties.OVERHEAD_LIGHT_NAME + ":state:toggle");
                playKeyBeep(1);
                fadeButton(btnLights);
            }
        });

        btnLights.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                fadeButton(btnLights);
                changeConfigurationPanel(pnlLights);
                playKeyBeep(4);
                return true;
            }
        });
    }
    @SuppressLint("ClickableViewAccessibility")
    void setLightPanelClickListeners() {
        btnPIR_lights.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                wifiCommunication.send("pir:lights:toggle");
                playKeyBeep(2);
                fadeButton(btnPIR_lights);
            }
        });

        btnLDR_lights.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                wifiCommunication.send("ldr:lights:toggle");
                playKeyBeep(2);
                fadeButton(btnLDR_lights);
            }
        });

        btnIncreaseLight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                incrementLightLevel(1);
                playKeyBeep(3);
                wifiCommunication.send(RoomProperties.OVERHEAD_LIGHT_NAME + ":brightness:" + overheadLightDimmerValues[lightLevel]);
                fadeButton(btnIncreaseLight);
            }
        });

        btnDecreaseLight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                incrementLightLevel(0);
                playKeyBeep(3);
                wifiCommunication.send(RoomProperties.OVERHEAD_LIGHT_NAME + ":brightness:" + overheadLightDimmerValues[lightLevel]);
                fadeButton(btnDecreaseLight);
            }
        });

        btnIncreaseLight.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                changeLightLevel(10);
                playKeyBeep(2);
                wifiCommunication.send(RoomProperties.OVERHEAD_LIGHT_NAME + ":brightness:" + overheadLightDimmerValues[lightLevel]);
                fadeButton(btnIncreaseLight);
                return true;
            }
        });

        btnDecreaseLight.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                changeLightLevel(1);
                playKeyBeep(2);
                wifiCommunication.send(RoomProperties.OVERHEAD_LIGHT_NAME + ":brightness:" + overheadLightDimmerValues[lightLevel]);
                fadeButton(btnDecreaseLight);
                return true;
            }
        });

        btnODSIncrease_lp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(3);
                fadeButton(btnODSIncrease_lp);
                incrementLightODSTimeout(1);
                wifiCommunication.send("ods:timeout:" + lightODSTimeout);
            }
        });

        btnODSDecrease_lp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(3);
                fadeButton(btnODSDecrease_lp);
                incrementLightODSTimeout(0);
                wifiCommunication.send("ods:timeout:" + lightODSTimeout);
            }
        });

        pnlLights.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                int action = event.getAction();
                if (action != MotionEvent.ACTION_DOWN && action != MotionEvent.ACTION_MOVE && action != MotionEvent.ACTION_UP) return false;
                Rect hitRect = new Rect();
                ImageView button;
                for(int i = 1; i <= 10; i++) {
                    button = img_light_bars[i];
                    button.getHitRect(hitRect);
                    hitRect.set(hitRect.left, hitRect.top + 2, hitRect.right, hitRect.bottom + 2);
                    if(hitRect.contains((int)event.getX(), (int)event.getY())) {
                        if(!(overheadLightDimmerValues[lightLevel] == overheadLightDimmerValues[i])) {
                            changeLightLevel(i);
                            wifiCommunication.send(RoomProperties.OVERHEAD_LIGHT_NAME + ":brightness:" + overheadLightDimmerValues[i]);
                        }
                    }
                }
                return true;
            }
        });
    }
    void setFanBlockClickListeners() {
        btnFan.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                wifiCommunication.send(RoomProperties.CEILING_FAN_NAME + ":state:toggle");
                wifiCommunication.send("temp_control:off");
                playKeyBeep(1);
                fadeButton(btnFan);
            }
        });

        btnFan.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                fadeButton(btnFan);
                changeConfigurationPanel(pnlFan);
                playKeyBeep(4);
                return true;
            }
        });

        btnPIR_fan.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(2);
                fadeButton(btnPIR_fan);
            }
        });

        btnTEMP_fan.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(2);
                fadeButton(btnTEMP_fan);
            }
        });
    }
    void setFanPanelOnClickListeners() {
        btnLowFanSpeed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                wifiCommunication.send(RoomProperties.CEILING_FAN_NAME + ":speed:low");
                wifiCommunication.send("temp_control:off");
                playKeyBeep(2);
                fadeButton(btnLowFanSpeed);
            }
        });

        btnMediumFanSpeed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                wifiCommunication.send(RoomProperties.CEILING_FAN_NAME + ":speed:medium");
                wifiCommunication.send("temp_control:off");
                playKeyBeep(2);
                fadeButton(btnMediumFanSpeed);
            }
        });

        btnHighFanSpeed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                wifiCommunication.send(RoomProperties.CEILING_FAN_NAME + ":speed:high");
                wifiCommunication.send("temp_control:off");
                playKeyBeep(2);
                fadeButton(btnHighFanSpeed);
            }
        });

        btnIncreaseODS_fp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(3);
                fadeButton(btnIncreaseODS_fp);
                incrementFanODSTimeout(1);
                wifiCommunication.send("fan:ods:timeout:" + fanODSTimeout);
            }
        });

        btnDecreaseODS_fp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(3);
                fadeButton(btnDecreaseODS_fp);
                incrementFanODSTimeout(0);
                wifiCommunication.send("fan:ods:timeout:" + fanODSTimeout);
            }
        });

        btnIncreaseTemp_fp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(3);
                fadeButton(btnIncreaseTemp_fp);
                incrementTargetTemp(1);
                wifiCommunication.send("temp_control:target:" + targetTemp);
            }
        });

        btnDecreaseTemp_fp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(3);
                fadeButton(btnDecreaseTemp_fp);
                incrementTargetTemp(0);
                wifiCommunication.send("temp_control:target:" + targetTemp);
            }
        });

        btnTEMP_fan.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(2);
                fadeButton(btnTEMP_fan);
                wifiCommunication.send("temp_control:toggle");
            }
        });
    }
    void setAuxiliaryOnClickListeners() {
        btnNight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                fadeButton(btnNight);
                playKeyBeep(2);
                wifiCommunication.send("night_mode:state:toggle");
            }
        });

        btnNightClock.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playKeyBeep(2);
                nightClockVisibility(true);
            }
        });

        btnSleepPC.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                fadeButton(btnSleepPC);
                playKeyBeep(2);
                wifiCommunication.send("desktop_computer:sleep");
            }
        });

        btnWakePC.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                fadeButton(btnWakePC);
                playKeyBeep(2);
                wifiCommunication.send("desktop_computer:wake_up");
            }
        });

        btnDeskLEDs.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                fadeButton(btnDeskLEDs);
                playKeyBeep(2);
                wifiCommunication.send("desk_led:state:" + (1 - deskLEDState));
            }
        });

        btnVoice.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                fadeButton(btnVoice);
                playKeyBeep(2);
                wifiCommunication.send("voice:state:" + (1 - voiceState));
            }
        });

        nightClockLayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                nightClockVisibility(false);
            }
        });
    }
    void setConnectionPanelOnClickListeners() {
        btnReconnectBluetooth = findViewById(R.id.btnReconnectBluetooth);
        btnReconnectBluetooth.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //fadeButton(btnReconnectBluetooth);
                playKeyBeep(2);
                txtConnectionLost.setText("RECONNECTING...");
                new Timer().schedule(new TimerTask() {
                    @Override
                    public void run() {
                       //wifiCommunication.reConnectBluetooth();
                    }
                }, 500);
                //wifiCommunication.reConnectBluetooth();
            }
        });
    }
    void setMiscOnClickListeners() {

    }

    //On Create=====================================================================================
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "App Initialized");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_room_ui);
        context = getApplicationContext();

        configureObjects();

        setDecorView(); //Set orientation

        setHeaderViews();
        setLightBlockViews();
        setLightPanelViews();
        setFanBlockViews();
        setFanPanelViews();
        setAuxiliaryViews();
        setConnectionPanelViews();
        setMiscViews();

        setLightBlockClickListeners();
        setLightPanelClickListeners();
        setFanBlockClickListeners();
        setFanPanelOnClickListeners();
        setAuxiliaryOnClickListeners();
        setConnectionPanelOnClickListeners();
        setMiscOnClickListeners();
        
        configureFadeProperties();

        setStartingPanel(pnlLights);

        //changeLightLevel(5);
        //changeLightODSTimeout(10);
        //changeFanODSTimeout(10);
    }

    //On Resume=====================================================================================
    @Override
    protected void onResume() {
        super.onResume();
        wifiCommunication.connectSocket();
    }

    @Override
    protected void onPause() {
        wifiCommunication.closeSocket();
        super.onPause();
    }

    //Panels========================================================================================
    void changeConfigurationPanel(final View fadeInPanel) {
        if(fadeInPanel == currentPanel) {
            return;
        }
        View fadeOutPanel = currentPanel;
        fadeView(fadeOutPanel, 0);
        new Timer().schedule(new TimerTask() {
            @Override
            public void run() {
                Utils.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        fadeView(fadeInPanel, 1);
                    }
                });
            }
        }, panelFadeOut.getDuration());
        currentPanel = fadeInPanel;
    }
    //Header
    void updateAmbientTemperature(int temperature) {
        ambientTemp = temperature;
        txtAmbientTemp.setText(String.valueOf(ambientTemp) + "°F");
    }
    //Light Panel
    void changeLightLevel(int level) {
        lightLevel = level;
        for (int i = 1; i <= level; i++) {
            //img_light_bars[i].setImageResource(R.drawable.ic_light_bar);
            changeLightBarState(i, ON);
        }
        for (int i = level + 1; i <= 10; i++) {
            changeLightBarState(i, OFF);
        }
    }
    void incrementLightLevel(int direction) {
        if (direction == 0 && lightLevel > 1) {
            lightLevel--;
        } else if (direction == 1 && lightLevel < 10) {
            lightLevel++;
        }
        changeLightLevel(lightLevel);
    }
    void changeLightODSTimeout(final int timeout) {
        lightODSTimeout = timeout;
        txtODSValue_lp.setText(String.valueOf(timeout));
    }
    void incrementLightODSTimeout(int direction) {
        if(direction == 1 && lightODSTimeout < 60) {
            changeLightODSTimeout(lightODSTimeout + 1);
        }
        else if(direction == 0 && lightODSTimeout > 0) {
            changeLightODSTimeout(lightODSTimeout - 1);
        }
    }
    //Fan Panel
    void changeFanODSTimeout(final int timeout) {
        fanODSTimeout = timeout;
        Utils.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                txtODSValue_fp.setText(String.valueOf(timeout));
            }
        });
    }
    void incrementFanODSTimeout(int direction) {
        if(direction == 1 && fanODSTimeout < 60) {
            changeFanODSTimeout(fanODSTimeout + 1);
        }
        else if(direction == 0 && fanODSTimeout > 0) {
            changeFanODSTimeout(fanODSTimeout - 1);
        }
    }
    void changeTargetTemp(final int temperature) {
        targetTemp = temperature;
        txtTempValue_fp.setText(String.valueOf(targetTemp));
    }
    void incrementTargetTemp(int direction) {
        if(direction == 1 && targetTemp < 90) {
            changeTargetTemp(targetTemp + 1);
        }
        else if(direction == 0 && targetTemp > 60) {
            changeTargetTemp(targetTemp - 1);
        }
    }
    //Auxiliary Panel
    void nightClockVisibility(boolean visible) {
        if(visible) {
            nightClockLayout.setVisibility(View.VISIBLE);
            WindowManager.LayoutParams layoutParams = getWindow().getAttributes(); // Get Params
            layoutParams.screenBrightness = 0; // Set Value
            getWindow().setAttributes(layoutParams); // Set params
        }
        else {
            nightClockLayout.setVisibility(View.GONE);
            WindowManager.LayoutParams layoutParams = getWindow().getAttributes(); // Get Params
            layoutParams.screenBrightness = 75; // Set Value
            getWindow().setAttributes(layoutParams); // Set params
        }
    }

    //Connection Panel
    void connectionLost() {
        txtConnectionLost.setText("CONNECTION LOST");
        imgConnectionPanelBrackets.setImageResource(R.drawable.ic_connection_panel_brackets);
        changeConfigurationPanel(pnlConnectionLost);
    }

    //Animations====================================================================================
    void playKeyBeep(int key) {
        mediaPlayer.reset();
        switch (key) {
            case 1:
                mediaPlayer = MediaPlayer.create(context.getApplicationContext(), R.raw.one_beep);
                mediaPlayer.start();
                break;
            case 2:
                mediaPlayer = MediaPlayer.create(context.getApplicationContext(), R.raw.two_beep);
                mediaPlayer.start();
                break;
            case 3:
                mediaPlayer = MediaPlayer.create(context.getApplicationContext(), R.raw.three_beep);
                mediaPlayer.start();
                break;
            case 4:
                mediaPlayer = MediaPlayer.create(context.getApplicationContext(), R.raw.configuration_panel_change);
                mediaPlayer.start();
                break;
        }
    }
    void fadeView(final View view, int direction) {
        if (direction == 0) {
            view.setVisibility(View.VISIBLE);
            view.startAnimation(panelFadeOut);
            new Timer().schedule(new TimerTask() {
                @Override
                public void run() {
                    Utils.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            view.setVisibility(View.INVISIBLE);
                        }
                    });
                }
            }, panelFadeOut.getDuration());
        } else if (direction == 1) {
            view.setVisibility(View.VISIBLE);
            view.startAnimation(panelFadeIn);
        }
    }
    void fadeButton(ImageButton buttonPressed) {
        if (buttonPressed == btnLights) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_lights_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_lights_pressed);
        } else if (buttonPressed == btnPIR_lights || buttonPressed == btnLDR_lights) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_light_auxiliary_button_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_light_auxiliary_button_pressed);
        } else if (buttonPressed == btnDecreaseLight || buttonPressed == btnALSDecrease_lp) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_decrease_light_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_decrease_light_pressed);
        }
        else if(buttonPressed == btnODSDecrease_lp) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_light_panel_decrease_skinny_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_light_panel_decrease_skinny_pressed);
        } else if(buttonPressed == btnODSIncrease_lp) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_light_panel_increase_skinny_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_light_panel_increase_skinny_pressed);
        } else if (buttonPressed == btnIncreaseLight || buttonPressed == btnALSIncrease_lp) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_increase_light_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_increase_light_pressed);
        } else if (buttonPressed == btnFan) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_fan_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_fan_pressed);
        } else if (buttonPressed == btnPIR_fan || buttonPressed == btnTEMP_fan) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_fan_auxiliary_button_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_fan_auxiliary_button_pressed);
        } else if (buttonPressed == btnLowFanSpeed || buttonPressed == btnMediumFanSpeed || buttonPressed == btnHighFanSpeed) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_fan_speed_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_fan_speed_pressed);
        }
        else if (buttonPressed == btnIncreaseODS_fp || buttonPressed == btnIncreaseTemp_fp) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_fan_panel_increase_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_fan_panel_increase_pressed);
        }
        else if (buttonPressed == btnDecreaseODS_fp || buttonPressed == btnDecreaseTemp_fp) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_fan_panel_decrease_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_fan_panel_decrease_pressed);
        }
        else if(buttonPressed == btnReconnectBluetooth) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_connection_panel_reconnect_unpressed);
            buttonDrawables[1] = getDrawable(R.drawable.ic_connection_panel_reconnect_pressed);
        }
        else if(buttonPressed == btnNight || buttonPressed == btnVoice) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_auxiliary_button_unpressed_color1);
            buttonDrawables[1] = getDrawable(R.drawable.ic_auxiliary_button_pressed_color1);
        }
        else if(buttonPressed == btnDeskLEDs || buttonPressed == btnSleepPC || buttonPressed == btnWakePC) {
            buttonDrawables[0] = getDrawable(R.drawable.ic_auxiliary_button_unpressed_color2);
            buttonDrawables[1] = getDrawable(R.drawable.ic_auxiliary_button_pressed_color2);
        }
        buttonClickFader[currentFader[0]] = new TransitionDrawable(buttonDrawables);
        buttonClickFader[currentFader[0]].setCrossFadeEnabled(true);
        buttonPressed.setImageDrawable(buttonClickFader[currentFader[0]]);
        buttonClickFader[currentFader[0]].startTransition(125);
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                buttonClickFader[delayedFader[0]].reverseTransition(75);
                delayedFader[0]++;
                if (delayedFader[0] >= 10) {
                    delayedFader[0] = 0;
                }
            }
        }, 100);
        currentFader[0]++;
        if (currentFader[0] >= 10) {
            currentFader[0] = 0;
        }
    }
    void updateIndicator(final ImageView indicator, final int state) {
        Utils.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (state == 1 && indicator.getVisibility() != View.VISIBLE) {
                    indicator.startAnimation(indicatorFadeIn);
                    new Timer().schedule(new TimerTask() {
                        @Override
                        public void run() {
                            Utils.runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    indicator.setVisibility(View.VISIBLE);
                                }
                            });
                        }
                    }, indicatorFadeIn.getDuration());
                }
                else if(state == 0 && indicator.getVisibility() != View.GONE) {
                    indicator.startAnimation(indicatorFadeOut);
                    new Timer().schedule(new TimerTask() {
                        @Override
                        public void run() {
                            Utils.runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    indicator.setVisibility(View.GONE);
                                }
                            });                        }
                    }, indicatorFadeOut.getDuration());
                }
            }
        });
    }
    void changeLightBarState(int bar, int state) {
        switch (state) {
            case OFF:
                if (lightBarStates[bar] == 1) {
                    lightBarFader[bar].reverseTransition(200);
                }
                lightBarStates[bar] = 0;
                break;
            case ON:
                if (lightBarStates[bar] == 0) {
                    lightBarFader[bar].startTransition(200);
                }
                lightBarStates[bar] = 1;
                break;
        }
    }

    //Device Functions==============================================================================
    void displayOff() {
    }
    void displayOn() {
    }
}
