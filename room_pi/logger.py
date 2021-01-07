from datetime import datetime

debug = False
log_file = open("/home/pi/room_pi/logs/room_pi.log", "a")


def log(message: str):
    time_stamp = datetime.now().strftime("%Y:%m:%d %H:%M:%S\t ")
    log_string = time_stamp + message

    if debug or "debug:" in message:
        print(log_string)
    else:
        log_file.write(log_string + "\n")
