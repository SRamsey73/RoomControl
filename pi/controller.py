import serial
import time
import measure_time
import socket
import select
import threading



# Controllers to interact with Peripherals
class Controller:

    SERIAL_CONNECTION = "serial"
    SOCKET_CONNECTION = "socket"

    def __init__(self, name: str, connection_type: str, port, connection_to_update):
        # Set name of controller
        self.name = name

        # List of peripherals connected to the controller
        self.room_peripherals = []

        # Set connection to peripheral: Socket/Serial
        # Check if connection is socket or serial
        if connection_type == "serial": 
            # Set conneciton to controller
            self.connection = SerialConnection(port)
        elif connection_type == "socket":
            self.connection = SocketConnection(port)

        # Any messages received by the controllers will be forwarded to this connection, likely UI
        self.connection_to_update = connection_to_update


    def read(self):
        msg = self.connection.read()
        if type(msg) is str:
            # call as a peripheral function
            self.call_peripheral_function(msg, False)
            # update other connections interested in state of controller
            self.connection_to_update.send(msg)
            print(msg)


    def call_peripheral_function(self, call_string: str, update_controller = True):
        if not (call_string is None):
            # Loop through peripherals connected to controller to resolve which message was for
            for peripheral in self.room_peripherals:
                # Check if first parameter in message is the name of the peripheral
                try:
                    if call_string.startswith(peripheral.name + ":"):
                        # This is the name of the peripheral, pass the remainder of the msg to the peripheral
                        peripheral.call_remote_function(call_string[len(peripheral.name) + 1:], update_controller)
                except ValueError:
                    # Peripheral name did not match
                    pass                    
    

    def close(self):
        self.connection.close()
        print("Connection to " + self.name.upper() + " closed.")



class SocketConnection:
    def __init__(self, port):
        self.port = port

        self.listener_socket = None
        self.configure_listener()

        self.connected_sockets = []
        self.connected_sockets.append(self.listener_socket)


    # Listen for and accept connections
    def configure_listener(self):
        self.listener_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.listener_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.listener_socket.bind(('192.168.4.1', self.port))
        self.listener_socket.listen()
                   

    def del_socket_by_addr(self, addr):
        for s in self.connected_sockets:
            if s.getpeername()[0] == addr:
                self.connected_sockets.remove(s)


    def del_socket_by_descriptor(self, descriptor):
        for s in self.connected_sockets:
            if s == descriptor:
                print("Socket deleted with address: " + str(s.getpeername()[0]))
                s.close()
                self.connected_sockets.remove(s)     


    # Listen for data written to socket
    def read(self) -> str:
        readable = select.select(self.connected_sockets, [], [], 0)[0]
        for s in readable:
            # Check if activity on listener socket, if so accept connection and return
            if s == self.listener_socket:
                conn, addr = self.listener_socket.accept()
                print("Socket connected with address: " + str(addr[0]))
                self.connected_sockets.append(conn)
                return
            
            # Socket was not a listener, incoming data from connected socket, read it
            msg = ""

            # Read message on socket
            while True:
                char = s.recv(1).decode('utf-8')
                if char == '':
                    self.del_socket_by_descriptor(s)
                    break
                elif char == '\4':
                    return msg
                else:
                    msg += char
        return None


    def send(self, msg, target_socket=None):
       
        # Check if more sockets than just the listener are connected
        if len(self.connected_sockets) > 1:
            for socket in self.connected_sockets[1:]:
                msg += '\4'
                socket.send(msg.encode('utf-8'))
         # Check if a specific socket is being targeted, if not send to all
        elif not target_socket == None:
            msg += '\4'
            target_socket.send(msg.encode('utf-8'))


    def close(self):
        for s in self.connected_sockets:
            s.close()
        


class SerialConnection:
    def __init__(self, port):
        self.serial_port = serial.Serial()
        self.serial_port.baudrate = 19200
        self.serial_port.port = port
        self.serial_port.open()
        self.serial_port.timeout = 1


    def read(self) -> str:
        # Check if serial is available
        if self.serial_port.in_waiting > 0:
            msg = self.serial_port.read_until(b'\4').decode('ascii')
            return msg[:-1]
        return None
    

    def send(self, msg):
        msg += '\4'
        #print(msg)
        self.serial_port.write(msg.encode('ascii'))

    
    def close(self):
        self.serial_port.close()
    