cd "I:\Projects\Room_Ver_2\arduino\desk_controller\d_c_master\"
"C:\Users\Seth Ramsey\.platformio\penv\Scripts\platformio.exe" run
scp .pio\build\megaatmega2560\firmware.hex pi@roompi:/home/pi/SmartRoom/uploader/desk_firmware.hex

:repeat
ssh pi@roompi "fuser -k /dev/ttyACM0 ; /home/pi/SmartRoom/uploader/upload.sh desk && rm /home/pi/SmartRoom/uploader/desk_firmware.hex" || goto :repeat
echo Compile and upload complete.