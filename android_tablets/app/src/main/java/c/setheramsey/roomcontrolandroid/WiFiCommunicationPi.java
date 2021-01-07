package c.setheramsey.roomcontrolandroid;

import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Writer;
import java.net.Socket;

public class WiFiCommunicationPi {
    final String TAG = "WiFiCommunication";
    final int OFF = 0;
    final int ON = 1;
    final int TOGGLE = 2;

    Socket socket;
    OutputStream socketOutput;
    Writer socketStringOutput;
    InputStream socketInput;
    BufferedReader socketStringInput;


    private RoomUI mRoomUi;

    Boolean inputStreamFlag = false;

    WiFiCommunicationPi(RoomUI roomUI) {
        Log.d(TAG, "WiFi Communication");
        mRoomUi = roomUI;
    }

    int intFromMessage(String msg) {
        return Integer.parseInt(msg.substring(msg.lastIndexOf(":") + 1));
    }

    void connectSocket() {
            Log.d(TAG, "Connecting socket...");
            if(socket != null) {
                try {
                    socket.close();
                }
                catch(IOException e) {

                }
            }
            new Thread() {
                public void run() {
                    try {
                        socket = new Socket("192.168.4.1", 7500);
                        socketOutput = socket.getOutputStream();
                        socketStringOutput = new BufferedWriter(new OutputStreamWriter(socketOutput, "UTF8"));
                        socketStringOutput = new PrintWriter(socketOutput);
                        socketInput = socket.getInputStream();
                    } catch (IOException e) {
                        Log.d(TAG, "Socket connection error");
                        Log.d(TAG, e.toString());
                        return;
                    }
                    startInputHandlerThread();
                    sync();
                }
            }.start();
    }

    void closeSocket() {
        inputStreamFlag = false;
        if (socket == null) {
            return;
        }
        try {
            socket.close();
        }
        catch (IOException e) {

        }
        socket = null;
    }

    public void write(int x, int y) {

    }

    public void send(String message) {
        if(socket == null || !socket.isConnected()) {
            return;
        }
        try {
            socketStringOutput.append(message);
            socketStringOutput.append('\4');
            socketStringOutput.flush();
        }
        catch (IOException e) {
            Log.d(TAG, e.getMessage());
        }

    }

    private void startInputHandlerThread () {
        inputStreamFlag = true;
        Thread inputStreamHandler = new Thread() {
            public void run() {
                while (inputStreamFlag) {
                    try {
                        if (socketInput.available() > 0) {
                            int ch;
                            int index = 0;
                            StringBuilder sb = new StringBuilder();

                            while (true) {
                                ch = socketInput.read();
                                if( (char) ch == '\4' ) {
                                    break;
                                }
                                else {
                                    sb.append((char) ch);
                                }
                                index++;
                            }
                            final String msg = sb.toString();

                            //Light commands
                            if (msg.startsWith(RoomProperties.OVERHEAD_LIGHT_NAME + ":")) {
                                //State changes
                                if(msg.contains(":state:")) {
                                    if(msg.contains("off")) {
                                        mRoomUi.updateIndicator(mRoomUi.imgLightsIndicator, OFF);
                                    }
                                    else if(msg.contains("on")) {
                                        mRoomUi.updateIndicator(mRoomUi.imgLightsIndicator, ON);
                                    }
                                }
                                //Brightness changes
                                else if(msg.contains(":brightness:")) {
                                    int brightnessLevel = intFromMessage(msg);
                                    int previousDifference = Math.abs(mRoomUi.overheadLightDimmerValues[0] - brightnessLevel);
                                    int currentDifference;
                                    if(Math.abs(mRoomUi.overheadLightDimmerValues[mRoomUi.lightLevel] - brightnessLevel) > 5) {
                                        for(int i = 1; i < 11; i++) {
                                            currentDifference = Math.abs(mRoomUi.overheadLightDimmerValues[i] - brightnessLevel);
                                            if(previousDifference < currentDifference) {
                                                if(!(i == 1)) {
                                                    mRoomUi.lightLevel = i - 1;
                                                }
                                                break;
                                            }
                                            if(i == 10) {
                                                mRoomUi.lightLevel = 10;
                                            }
                                            previousDifference = currentDifference;
                                        }
                                        Utils.runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mRoomUi.changeLightLevel(mRoomUi.lightLevel);
                                            }
                                        });
                                    }
                                }
                            }

                            //Fan commands
                            else if (msg.startsWith(RoomProperties.CEILING_FAN_NAME + ":")) {
                                //State changes
                                if(msg.contains(":state:")) {
                                    if(msg.contains("off")) {
                                        mRoomUi.updateIndicator(mRoomUi.imgFanIndicator, OFF);
                                    }
                                    else if(msg.contains("on")) {
                                        mRoomUi.updateIndicator(mRoomUi.imgFanIndicator, ON);
                                    }
                                }
                                else if(msg.contains(":speed:")) {
                                    if(msg.contains(":low")) {
                                        mRoomUi.updateIndicator(mRoomUi.imgLowFanSpeedIndicator, ON);
                                        mRoomUi.updateIndicator(mRoomUi.imgMediumFanSpeedIndicator, OFF);
                                        mRoomUi.updateIndicator(mRoomUi.imgHighFanSpeedIndicator, OFF);
                                    }
                                    if(msg.contains(":medium")) {
                                        mRoomUi.updateIndicator(mRoomUi.imgLowFanSpeedIndicator, OFF);
                                        mRoomUi.updateIndicator(mRoomUi.imgMediumFanSpeedIndicator, ON);
                                        mRoomUi.updateIndicator(mRoomUi.imgHighFanSpeedIndicator, OFF);
                                    }
                                    if(msg.contains(":high")) {
                                        mRoomUi.updateIndicator(mRoomUi.imgLowFanSpeedIndicator, OFF);
                                        mRoomUi.updateIndicator(mRoomUi.imgMediumFanSpeedIndicator, OFF);
                                        mRoomUi.updateIndicator(mRoomUi.imgHighFanSpeedIndicator, ON);
                                    }
                                }
                            }

                            //Auxiliary commands
                            //Night mode
                            else if(msg.contains("night_mode:state:")) {
                                if(msg.contains("off")) {
                                    mRoomUi.updateIndicator(mRoomUi.imgNightIndicator, OFF);
                                }
                                else if(msg.contains("on")) {
                                    mRoomUi.updateIndicator(mRoomUi.imgNightIndicator, ON);
                                }
                            }
                            
                            //Room metrics
                            //temperature
                            else if(msg.contains("temperature_sensor:temperature:")) {
                                Utils.runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        mRoomUi.updateAmbientTemperature(intFromMessage(msg));
                                    }
                                });
                            }
                        }
                    } catch (IOException e) {
                        Log.d(TAG, e.getMessage());
                    }
                }
            }
        };
        inputStreamHandler.start();
    }

    void sync() {
        send("sync");
    }

}
