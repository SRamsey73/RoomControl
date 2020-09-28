import time

global start_time
global stop_time

def start():
    global start_time
    start_time = time.time()

def stop():
    global stop_time
    stop_time = time.time()

def print_elapsed():
    global start_time
    global stop_time
    print(stop_time - start_time)