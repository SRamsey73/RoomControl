from controller import SocketConnection

class ControlInterface:
    def __init__(self, name: str, port: int, read_actions: callable):
        self.name = name
        self.port = port
        self.connection = SocketConnection(self.port)
        self.read_actions = read_actions

    
    def read(self):
        # read data on connection
        msg = self.connection.read()

        # if read retuned a message it needs interpreted
        if type(msg) is str:
            # call read_actions with the msg
            self.read_actions(msg)
            print(msg)


    def close(self):
        self.connection.close()
        print("Connection to " + self.name.upper() + " closed.")