from datetime import datetime

log_file = open("./logs/room_pi.log", "a")

def log(message: str):
    time_stamp = datetime.now().strftime("%Y:%m:%d %H:%M:%S\t ")
    log_file.write(time_stamp + message + "\n")

