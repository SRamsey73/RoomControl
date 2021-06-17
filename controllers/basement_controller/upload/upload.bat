@echo off
cd "I:\Projects\Room_Control\controllers\basement_controller\"
"C:\Users\Seth Ramsey\.platformio\penv\Scripts\platformio.exe" run
scp .pio\build\nanoatmega328\firmware.hex RoomPi:/home/pi/room_pi/uploader/basement_firmware.hex

:repeat
ssh RoomPi "/home/pi/room_pi/uploader/upload.sh basement && rm /home/pi/room_pi/uploader/basement_firmware.hex" || goto :repeat
echo Compile and upload complete.