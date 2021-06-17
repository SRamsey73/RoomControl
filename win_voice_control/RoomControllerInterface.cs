using Room_Control__PC_;
using System;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

public class RoomControllerInterface
{
    Socket client;
    IPEndPoint remoteEP;

    public class StateObject
    {
        // Client socket.  
        public Socket workSocket = null;
        // Size of receive buffer.  
        public const int BufferSize = 256;
        // Receive buffer.  
        public byte[] buffer = new byte[BufferSize];
        // Received data string.
        public StringBuilder sb = new StringBuilder();
    }

    private static ManualResetEvent connectDone = new ManualResetEvent(false);

    public RoomControllerInterface(string name, IPEndPoint remoteEP) {
        this.remoteEP = remoteEP;
        this.client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        attemptConnection(this.client, this.remoteEP);
    }

    public static void attemptConnection(Socket client, IPEndPoint remoteEP)
    {
        while (!client.Connected) { 
            client.BeginConnect(remoteEP, new AsyncCallback(ConnectCallback), client);
            connectDone.WaitOne();
            connectDone.Reset();
        }
        Receive(client);
    }
    public void close(bool permanently = false)
    {
        close(this.client, permanently);
    }

    public static void close(Socket client, bool permanently = false)
    {
        if (permanently)
        {
            client.Dispose();
        }
        else
        {
            client.Shutdown(SocketShutdown.Both);
            client.Disconnect(true);
        }
    }

    public void send(string data)
    {
        send(this.client, data);
    }

    private static void send(Socket client, string data)
    {
        byte[] byteData = Encoding.ASCII.GetBytes(data + '\u0004');
        try
        {
            client.BeginSend(byteData, 0, byteData.Length, SocketFlags.None, new System.AsyncCallback(SendCallback), client);
        }
        catch (SocketException e) { Debug.WriteLine(e.ToString()); }
    } 

    private static void Receive(Socket client)
    {
        try
        {
            // Create the state object.  
            StateObject state = new StateObject();
            state.workSocket = client;

            // Begin receiving the data from the remote device.  
            client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                new AsyncCallback(ReceiveCallback), state);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    private static void ConnectCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the socket from the state object.  
            Socket client = (Socket)ar.AsyncState;

            // Complete the connection.
            client.EndConnect(ar);

            Console.WriteLine("Socket connected to {0}",
                client.RemoteEndPoint.ToString());
        }
        catch (SocketException)
        {
            Console.WriteLine("Connection failed, retrying in 5s");
            Thread.Sleep(5000);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
        connectDone.Set();
    }

    private static void ReceiveCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the state object and the client socket
            // from the asynchronous state object.  
            StateObject state = (StateObject)ar.AsyncState;
            Socket client = state.workSocket;
            // Read data from the remote device.  
            int bytesRead = client.EndReceive(ar);

            // There might be more data, so store the data received so far.  
            state.sb.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesRead));

            // All the data has arrived; put it in response.  
            if (state.sb.Length > 1)
            {
                string message = state.sb.ToString();
                Debug.WriteLine("Message received: " + state.sb.ToString());
                if (message.StartsWith("desktop_computer:sleep"))
                {
                    Program.mRoomInteractions.sleepPC();
                }
                else if(message.StartsWith("desktop_computer:restart_voice")) {
                    Program.mVoiceRecogniton.configureRecEngine();
                }

                state.sb.Clear();
            }

            //Read next message
            try
            {
                client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                        new AsyncCallback(ReceiveCallback), state);
            } catch(SocketException)
            {
                IPEndPoint remoteEP = (IPEndPoint) client.RemoteEndPoint;
                close(client);
                attemptConnection(client, remoteEP);
            }
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    private static void SendCallback(System.IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;

                int bytesSent = client.EndSend(ar);
            } catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
    }
