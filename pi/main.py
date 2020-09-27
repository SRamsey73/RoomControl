#!/usr/bin/env python3
from controller import Controller
from room_peripherals import Light, Fan, TemperatureSensor, OccupancySensor, LightSensor
from control_interface import ControlInterface
import os
from threading import Thread, main_thread


# Constants
LIGHT_THRESHOLD = 500


def handle_user_input():
    os.system('clear')
    print("Smart Room Hub | Raspberry Pi")
    print("=============================")

    while True:
        try:
            command = input('> ')
            if command == "quit":
                print("\nQuiting applicaiton...")
                exit_application()
                break
            
            if "bc" in command:
                basement_controller.call_peripheral_function(command[3:])
            elif "dc" in command:
                desk_controller.connection.send(command[3:])
                pass
            elif "light" in command:
                if "off" in command:
                    overhead_light.set_state(Light.OFF)
                if "on" in command:
                    overhead_light.set_state(Light.ON)
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
    tablets.close()
    basement_controller.close()
    desk_controller.close()


# Room state variables
night_mode = False
occupancy_enabled = True
light_level_enabled = True


def tablet_read_actions(msg: str):
    global night_mode
    # check if sync was requested
    if msg == "sync":
        sync_tablets()
    # night mode
    elif msg.startswith("night_mode:state:"):
        if ":off" in msg:
            night_mode = False
        elif ":on" in msg:
            night_mode = True
        elif ":toggle" in msg:
            night_mode = not night_mode
        else:
            return
        tablets.connection.send("night_mode:state:" + ("on" if night_mode else "off"))
    else:
        # state changes to make on controllers, these are functions in classes like Light, Fan, or OccupancySensor and registered to controllers
        return basement_controller.call_peripheral_function(msg) or desk_controller.call_peripheral_function(msg)
    
    print(msg)



def sync_tablets():
    # send tablets information needed to update UI
    # overhead light
    tablets.connection.send(overhead_light.name + ":state:" + ("on" if overhead_light.get_state() else "off"))
    tablets.connection.send(overhead_light.name + ":brightness:" + str(overhead_light.get_brightness()))

    # fan
    tablets.connection.send(ceiling_fan.name + ":state:" + ("on" if ceiling_fan.get_state() else "off"))
    tablets.connection.send(ceiling_fan.name + ":speed:" + ceiling_fan.get_speed())

    # room temperature
    tablets.connection.send(temperature_sensor.name + ":temperature:" + str(temperature_sensor.get_temperature()))

    # night mode
    global night_mode
    tablets.connection.send("night_mode:state:" + ("on" if night_mode else "off"))


def occupancy_changed():
    global LIGHT_THRESHOLD
    global night_mode
    global occupancy_enabled
    global light_level_enabled

    # Check if occupancy sensor is enabled
    if occupancy_enabled:
        # Check if room is occupied and not in night mode
        if occupancy_sensor.get_occupied() and not night_mode:
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



def temperature_changed():
    pass

def light_level_changed():
    pass


# Create tablets
tablets = ControlInterface("tablets", 7500, tablet_read_actions)

# Create controllers that interact with the room
basement_controller = Controller("basement_controller", Controller.SERIAL_CONNECTION, "/dev/ttyUSB0", tablets.connection)
desk_controller = Controller("desk_controller", Controller.SERIAL_CONNECTION, "/dev/ttyACM0", tablets.connection)

# Create room peripherals
# Basement controller
overhead_light = Light("overhead_light", basement_controller)
ceiling_fan = Fan("ceiling_fan", basement_controller)
# Desk controller
occupancy_sensor = OccupancySensor("occupancy_sensor", desk_controller, occupancy_changed)
temperature_sensor = TemperatureSensor("temperature_sensor", desk_controller, temperature_changed)
light_levelsensor = LightSensor("light_sensor", desk_controller, light_level_changed)


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
        tablets.read()
    
    main_kill_flag=False


        #TODO Take action on current state of program



    