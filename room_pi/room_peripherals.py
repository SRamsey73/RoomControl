from controller import Controller

class RoomPeripheral:
    OFF = "off"
    ON = "on"
    TOGGLE = "toggle"

    def __init__(self):
        self.remote_function_map = {}

    def call_remote_function(self, call_string: str, update_controller = True):
        call_string = call_string.split(":")

        try:
            self.remote_function_map[call_string[0]](call_string[1], update_controller)
        except KeyError:
            # No match to update parameter
            pass


# Room peripheral classes
class Light(RoomPeripheral):
    TOGGLE = "toggle"
    def __init__(self, name: str, controller: Controller):
        # State variables of light
        self._state = False
        self._brightness = 100

        # Set name of the peripheral
        self.name = name

        # Set controller connected to light
        self.controller = controller
        # Add peripheral to controller
        self.controller.room_peripherals.append(self)

        # Map functions to call strings
        self.remote_function_map = {"state": self.set_state, "brightness": self.set_brightness}


    # Getter and setter for state
    def get_state(self):
        return self._state


    def set_state(self, state, update_controller: bool = True):
        if state == False or state == "off":
            # Set state to off
            self._state = False
        elif state == True or state == "on":
            # Set state to on
            self._state = True
        elif state == "toggle":
            # Toggle state
            self.set_state(not self._state)
            return
        else:
            # State invalid
            return
        
        # Send new state over serial
        if update_controller:
            self.controller.connection.send(self.name + ":state:" + ("on" if self._state else "off"))

    
    # Getter and setter for brightness
    def get_brightness(self):
        return self._brightness


    def set_brightness(self, brightness: int or str, update_controller: bool = True):
        # Check if brightness is a string and convert to int
        if type(brightness) is str:
            try:
                brightness = int(brightness)
            except ValueError:
                return

        # Check if brightness is in range
        if 0 <= brightness <= 100:
            # Set new brightness
            self._brightness = brightness
            # Send new brightness over serial
            if update_controller:
                self.controller.connection.send(self.name + ":brightness:" + str(brightness))
        else:
            # brightness not in range
            return



class Fan(RoomPeripheral):
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    
    def __init__(self, name: str, controller: Controller):
        # State variables of light
        self._state = False
        self._speed = Fan.LOW

        # Set name of the peripheral
        self.name = name

        # Set controller connected to light
        self.controller = controller
        # Add peripheral to controller
        self.controller.room_peripherals.append(self)

        # Map functions to call strings
        self.remote_function_map = {"state": self.set_state, "speed": self.set_speed}


    # Getter and setter for state
    def get_state(self):
        return self._state


    def set_state(self, state, update_controller: bool = True):
        if state == False or state == Fan.OFF:
            # Set state to off
            self._state = False
        elif state == True or state == Fan.ON:
            # Set state to on
            self._state = True
        elif state == Fan.TOGGLE:
            # Toggle state
            self.set_state(not self._state)
            return
        else:
            # State invalid
            return
        
        # Send new state over serial
        if update_controller:
            self.controller.connection.send(self.name + ":state:" + ("on" if self._state else "off"))

    
    # Getter and setter for speed
    def get_speed(self):
        return self._speed


    def set_speed(self, speed: str, update_controller: bool = True):
        # Check if speed is valid
        if speed == Fan.LOW or Fan.MEDIUM or Fan.HIGH:
            # Set new speed
            self._speed = speed
            # Send new speed over serial
            if update_controller:
                self.controller.connection.send(self.name + ":speed:" + speed)
        else:
            # speed not in range
            return



class LightSensor(RoomPeripheral):
    def __init__(self, name, controller: Controller, callback: callable):
        # set name of sensor
        self.name = name
        # set controller attached to light sensor
        self.controller = controller
        # Add peripheral to controller
        self.controller.room_peripherals.append(self)
        # light level variable
        self._light_level = 0
        # Map functions to call strings
        self.remote_function_map = { "light_level": self.set_light_level }
        # Callback when sensor state changes
        self._callback = callback
    

    def get_light_level(self):
        return self._light_level


    def set_light_level(self, light_level, manual_update=True):
        if type(light_level) is int or str:
            try:
                self._light_level = int(light_level)
            except ValueError:
                return
            self._callback()

    
    def set_callback(self, callback):
        self._callback = callback



class TemperatureSensor(RoomPeripheral):
    def __init__(self, name, controller: Controller, callback: callable):
        # set name of sensor
        self.name = name
        # set controller attached to light sensor
        self.controller = controller
        # Add peripheral to controller
        self.controller.room_peripherals.append(self)
        # light level variable
        self._temperature = 0
        # Map functions to call strings
        self.remote_function_map = { "temperature": self.set_temperature }
        # Callback when sensor state changes
        self._callback = callback
    

    def get_temperature(self):
        return self._temperature


    def set_temperature(self, temperature, manual_update=True):
        if type(temperature) is int or str:
            try:
                self._temperature = int(temperature)
            except ValueError:
                return
            self._callback()

    
    def set_callback(self, callback):
        self._callback = callback



class OccupancySensor(RoomPeripheral):
    def __init__(self, name, controller: Controller, callback: callable):
        # set name of sensor
        self.name = name
        # set controller attached to light sensor
        self.controller = controller
        # Add peripheral to controller
        self.controller.room_peripherals.append(self)
        # light level variable
        self._occupied = False
        # Map functions to call strings
        self.remote_function_map = { "occupied": self.set_occupied }
        # Callback when sensor state changes
        self._callback = callback


    def get_occupied(self):
        return self._occupied


    def set_occupied(self, occupied, manual_update=True):
        if type(occupied) is bool or str:
            if not occupied or occupied == "false":
                self._occupied = False
            elif occupied or occupied == "true":
                self._occupied = True
            else:
                return
            self._callback()
    

    def set_callback(self, callback):
        self._callback = callback



class LEDStrip(RoomPeripheral):
    def __init__(self, name, controller: Controller, animations: (str)):
            # set name of strip
            self.name = name
            # set controller to interact with LEDStrip
            self.controller = controller
            # Add peripheral to controller
            self.controller.room_peripherals.append(self)
            # Animations strip can display
            self._animations = animations
            # Current animation strip is displaying
            self._current_animation = animations[0]
            # Map functions to call strings
            self.remote_function_map = { 
                "state": self.set_state,
                "animation": self.set_current_animation 
            }

            self._state = True


    def set_state(self, state, update_controller = True):
        if (type(state) is bool and state) or state == RoomPeripheral.ON:
            self._state = True
        elif (type(state) is bool and not state) or state == RoomPeripheral.OFF:
            self._state = False
        elif state == RoomPeripheral.TOGGLE:
            self.set_state(not state)
            return
        else:
            return
        if(update_controller):
            self.controller.connection.send(self.name + ":state:" + ("on" if self._state else "off"))


    
    def set_current_animation(self, animation_name, update_controller = True):
        if animation_name in self._animations:
            self._current_animation = animation_name
            if(update_controller):
                self.controller.connection.send(self.name + ":animation:" + animation_name)
