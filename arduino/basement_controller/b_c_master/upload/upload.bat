cd "I:\Projects\Room_Ver_2\arduino\basement_controller\b_c_master\"
"C:\Users\Seth Ramsey\.platformio\penv\Scripts\platformio.exe" run
scp .pio\build\nanoatmega328\firmware.hex pi@roompi:/home/pi/SmartRoom/uploader/basement_firmware.hex
ssh pi@RoomPi "fuser -k /dev/ttyUSB0 ; /home/pi/SmartRoom/uploader/upload.sh basement && rm /home/pi/SmartRoom/uploader/basement_firmware.hex"