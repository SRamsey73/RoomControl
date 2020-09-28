@echo off
cd "I:\Projects\Room_Control\controllers\desk_controller\"
"C:\Users\Seth Ramsey\.platformio\penv\Scripts\platformio.exe" run
scp .pio\build\megaatmega2560\firmware.hex pi@roompi:/home/pi/room_pi/uploader/desk_firmware.hex

:repeat
ssh pi@roompi "/home/pi/room_pi/uploader/upload.sh desk && rm /home/pi/room_pi/uploader/desk_firmware.hex" || goto :repeat
echo Compile and upload complete.