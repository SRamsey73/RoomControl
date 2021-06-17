#!/usr/bin/env python3
from logger import log, log_file
from controller import Controller
from room_peripherals import Light, Fan, TemperatureSensor, OccupancySensor, LightSensor, LEDStrip
from control_interface import ControlInterface
import os
from threading import Thread, main_thread
from wakeonlan import send_magic_packet

log("\n\nApplication start")

# Constants
# Light levels less than this threshold will turn the overhead light on
LIGHT_THRESHOLD = 600

def handle_user_input():
    os.system('clear')
    print("Smart Room Hub | Raspberry Pi")
    print("=============================")

    while True:
        try:
            command = input('> ')
            if command == "quit":
                print("\nQuitting application...")
                exit_application()
                break
            if "detach" in command:
                os.system("screen -d")
                continue
            if "bc" in command:
                basement_controller.call_peripheral_function(command[3:])
            elif "dc" in command:
                desk_controller.connection.send(command[3:])
            elif "light" in command:
                if "off" in command:
                    overhead_light.set_state(Light.OFF)
                elif "on" in command:
                    overhead_light.set_state(Light.ON)
            elif "fan" in command:
                if "off" in command:
                    ceiling_fan.set_state(Fan.OFF)
                elif "on" in command:
                    ceiling_fan.set_state(Fan.ON)
            elif "desk_leds" in command:
                if "animation" in command:
                    desk_led_strip.set_current_animation(command[len("desk_leds animation "):])
                elif "state" in command:
                    if "off" in command:
                        desk_led_strip.set_state(LEDStrip.OFF)
                    elif "on" in command:
                        desk_led_strip.set_state(LEDStrip.ON)
            else:
                print("Command not recognised")
        except KeyboardInterrupt:
            pass


# actions to take to exit application
def exit_application():
    # update kill flag
    global main_kill_flag
    main_kill_flag = True
    # wait for main thread to terminate
    while main_kill_flag: pass
    control_interfaces.close()
    basement_controller.close()
    desk_controller.close()
    log_file.close()
    


# Room state variables
night_mode = False
occupancy_enabled = True
light_level_enabled = True


def controller_read_actions(msg: str):
    # check if sync was requested
    if msg == "sync":
        sync_tablets()
    # night mode
    elif msg.startswith("night_mode:state:"):
        if ":off" in msg:
            setNightMode(False)
        elif ":on" in msg:
            setNightMode(True)
        elif ":toggle" in msg:
            setNightMode(not night_mode)
        else:
            return
        control_interfaces.connection.send("night_mode:state:" + ("on" if night_mode else "off"))
    elif msg.startswith("desktop_computer:wake_up"):
        wake_desktop_computer()
    elif msg.startswith("desktop_computer:sleep"):
        sleep_desktop_computer()
    else:
        # state changes to make on controllers, these are functions in classes like Light, Fan, or OccupancySensor and registered to controllers
        return basement_controller.call_peripheral_function(msg) or desk_controller.call_peripheral_function(msg)
    
    log("Control interface: " + msg)



def sync_tablets():
    # send tablets information needed to update UI
    # overhead light
    control_interfaces.connection.send(overhead_light.name + ":state:" + ("on" if overhead_light.get_state() else "off"))
    control_interfaces.connection.send(overhead_light.name + ":brightness:" + str(overhead_light.get_brightness()))

    # fan
    control_interfaces.connection.send(ceiling_fan.name + ":state:" + ("on" if ceiling_fan.get_state() else "off"))
    control_interfaces.connection.send(ceiling_fan.name + ":speed:" + ceiling_fan.get_speed())

    # room temperature
    control_interfaces.connection.send(temperature_sensor.name + ":temperature:" + str(temperature_sensor.get_temperature()))

    # night mode
    global night_mode
    control_interfaces.connection.send("night_mode:state:" + ("on" if night_mode else "off"))


def occupancy_changed():
    global LIGHT_THRESHOLD
    global night_mode
    global occupancy_enabled
    global light_level_enabled

    # Check if occupancy sensor is enabled
    if occupancy_enabled:
        # Check if room is occupied and not in night mode
        if occupancy_sensor.get_occupied() and not night_mode:
            # Turn desk leds on
            desk_led_strip.set_state(LEDStrip.ON)

            # Logic to control overhead light that incorporates ambient light level
            # Check if light level is enabled 
            if light_level_enabled:
                # Check if light level is less than threshold
                if light_levelsensor.get_light_level() < LIGHT_THRESHOLD:
                    # Turn the light on
                    overhead_light.set_state(Light.ON)
                # Light level is already high enough, no need for light
                else:
                    return
            # Light level is not enabled
            else:
                #Turn the light on
                overhead_light.set_state(Light.ON)
            
        # Check if room is not occupied
        elif not occupancy_sensor.get_occupied():
            # Turn the light off
            overhead_light.set_state(Light.OFF)
            # Turn desk leds off
            desk_led_strip.set_state(LEDStrip.OFF)



def temperature_changed():
    pass

def light_level_changed():
    pass

def wake_desktop_computer():
    try:
        send_magic_packet("D0-50-99-8A-87-23")
    except OSError:
        pass

def sleep_desktop_computer():
    control_interfaces.connection.send("desktop_computer:sleep")

def setNightMode(new_night_mode):
    global night_mode
    if(type(new_night_mode) == bool):
        night_mode = new_night_mode
        if night_mode:
            desk_controller.connection.send("desk_leds:state:off")
        else:
            desk_controller.connection.send("desk_leds:state:on")



# Create tablets
control_interfaces = ControlInterface("control_interfaces", 7500, controller_read_actions)

# Create desktop computer for voice recognition

# Create controllers that interact with the room
basement_controller = Controller("basement_controller", Controller.SERIAL_CONNECTION, "/dev/ttyUSB0", control_interfaces.connection)
desk_controller = Controller("desk_controller", Controller.SERIAL_CONNECTION, "/dev/ttyACM0", control_interfaces.connection)

# Create room peripherals
# Basement controller
overhead_light = Light("overhead_light", basement_controller)
ceiling_fan = Fan("ceiling_fan", basement_controller)
# Desk controller
occupancy_sensor = OccupancySensor("occupancy_sensor", desk_controller, occupancy_changed)
temperature_sensor = TemperatureSensor("temperature_sensor", desk_controller, temperature_changed)
light_levelsensor = LightSensor("light_sensor", desk_controller, light_level_changed)
desk_led_strip = LEDStrip("desk_leds", desk_controller, ("tail_effect", "rwb_center"))


# Run if main
if __name__ == "__main__":
    # Kill flag to end spawned threads
    global main_kill_flag
    # Start thread to handle user input
    ui_thread = Thread(target=handle_user_input)
    ui_thread.start()

    # Event loop
    main_kill_flag = False
    while not main_kill_flag:
        # Read controller input
        basement_controller.read()
        desk_controller.read()
        control_interfaces.read()
    
    main_kill_flag=False


        #TODO Take action on current state of program



    
