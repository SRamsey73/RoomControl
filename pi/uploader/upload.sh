#!/bin/bash

cd /home/pi/SmartRoom2/uploader
if [[ $1 == 'desk' ]]; then
    avrdude -C avrdude.conf -v -patmega2560  -cwiring -P /dev/ttyACM0 -b 115200 -D -U flash:w:desk_firmware.hex:i
elif [[ $1 == 'basement' ]]; then
    avrdude -C avrdude.conf -v -p atmega328p -c arduino -P /dev/ttyUSB0 -b 57600 -D -U flash:w:basement_firmware.hex:i
fi