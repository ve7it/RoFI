using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;
using pb = global::Google.Protobuf;

using MessageQueue = System.Collections.Concurrent.ConcurrentQueue<System.Tuple<int, System.Collections.Generic.List<byte>>>;

class ClientInfo
{
    static private int nextId = 1;
    public int clientId{ get; private set; } // zero means no client assigned
    public TcpClient tcpClient;
    public NetworkStream stream;

    public List< byte > receivedMessage = new List< byte >();
    private int _sizeOfMsgLength = 0;
    public int SizeOfMsgLength
    {
        get
        {
            if ( _sizeOfMsgLength > 0 )
            {
                return _sizeOfMsgLength;
            }

            for ( int i = 0; i < receivedMessage.Count; i++ )
            {
                if ( ( receivedMessage[i] >> 7 ) == 0 )
                {
                    _sizeOfMsgLength = i + 1;
                    return _sizeOfMsgLength;
                }
            }

            return _sizeOfMsgLength;
        }
    }

    public ClientInfo()
    {
        clientId = nextId++;
    }

    private int GetMsgLength()
    {
        if ( SizeOfMsgLength == 0 )
        {
            return 0;
        }

        byte[] lengthBytes = new byte[ SizeOfMsgLength ];
        receivedMessage.CopyTo( 0, lengthBytes, 0, SizeOfMsgLength );
        return CodedInputStream( lengthBytes ).ReadLength();
    }

    public Tuple< int, List< byte > > getMessage()
    {
        int length = GetMsgLength();

        List< byte > newMsg = new List< byte >( receivedMessage.GetRange( SizeOfMsgLength, length ) );
        receivedMessage.RemoveRange( 0, SizeOfMsgLength + length );

        return Tuple.Create( clientId, newMsg );
    }

    public bool IsCompleteMessage()
    {
        return SizeOfMsgLength > 0 && receivedMessage.Count >= SizeOfMsgLength + GetMsgLength();
    }
}

class TCPServer
{
    public const string ipAdress = "127.0.0.1";
    public const int port = 54010;

    private TcpListener server;
    private Mutex serverActive = new Mutex();
    private bool serverInCreation = false;
    private List< ClientInfo > clients = new List< ClientInfo >();

    private MessageQueue receivedQueue;
    private MessageQueue sendQueue;

    private volatile bool closingConnections = false;
    private volatile bool closingServer = false;


    public TCPServer( MessageQueue received, MessageQueue send )
    {
        receivedQueue = received;
        sendQueue = send;
    }

    public bool StartServer()
    {
        if ( !serverActive.WaitOne( 0 ) )
        {
            return false;
        }
        if ( serverInCreation )
        {
            serverActive.ReleaseMutex();
            return false;
        }
        serverInCreation = true;
        serverActive.ReleaseMutex();

        IPAddress ip = IPAddress.Parse( ipAdress );
        server = new TcpListener( ip, port );
        server.Start();
        ServerLog( "Server Started on ip: " + ipAdress + ":" + port );

        Thread serverThread = new Thread( ServerTask );
        serverThread.Start();

        return true;
    }

    private void ServerTask()
    {
        serverActive.WaitOne();
        serverInCreation = false;

        while ( ShouldContinue() )
        {
            TryToConnectToClient();

            Tuple< int, List< byte > > sendMsg;
            while ( sendQueue.TryDequeue( out sendMsg ) )
            {
                SendMessageToClient( sendMsg.Item1, sendMsg.Item2 );
            }

            foreach ( ClientInfo clientInfo in clients )
            {
                TryReadMessageFromClient( clientInfo );
            }

            Thread.Sleep( 0 );
        }

        serverActive.ReleaseMutex();
    }
    private void TryToConnectToClient()
    {
        while ( server != null && server.Pending() )
        {
            ClientInfo clientInfo = new ClientInfo();
            clientInfo.tcpClient = server.AcceptTcpClient();
            clientInfo.stream = clientInfo.tcpClient.GetStream();

            clients.Add( clientInfo );
        }
    }

    private void TryReadMessageFromClient( ClientInfo clientInfo )
    {
        if ( !clientInfo.stream.DataAvailable )
        {
            return;
        }

        byte[] buffer = new byte[ 1024 ];
        int bytesReceived = clientInfo.stream.Read( buffer, 0, buffer.Length );

        Array.Resize( ref buffer, bytesReceived );
        ServerLog( "Got message: \"" + toASCII( buffer ) + "\"" +
                " (" + toHex( buffer ) + ")" );

        clientInfo.receivedMessage.AddRange( buffer );

        if ( clientInfo.IsCompleteMessage() )
        {
            receivedQueue.Enqueue( clientInfo.getMessage() );
        }
    }

    private NetworkStream GetStreamFromId( int clientId )
    {
        NetworkStream stream = null;
        for (int i = 0; i < clients.Count; i++)
        {
            if (clientId == clients[i].clientId)
            {
                stream = clients[i].stream;
            }
        }
        return stream;
    }

    private byte[] PrepareMessage( List< byte > message )
    {
        int msgSize = pb::CodedOutputStream.ComputeLengthSize( message.Count ) + message.Count;

        byte[] preparedMsg = new byte[ msgSize ];
        CodedOutputStream msgStream = new CodedOutputStream( preparedMsg );

        msgStream.WriteLength( message.Count );
        message.CopyTo( preparedMsg, preparedMsg.Length - message.Count );

        return preparedMsg;
    }

    private void SendMessageToClient( int clientId, List< byte > sendMsg )
    {
        NetworkStream stream = GetStreamFromId( clientId );
        if ( stream == null )
        {
            ServerLog( "No client with given id: " + clientId );
            return;
        }

        byte[] msgOut = PrepareMessage( sendMsg );

        ServerLog( "Sending message: \"" + toHex( msgOut ) + "\"" );
        stream.WriteAsync( msgOut, 0, msgOut.Length );
    }

    private bool ShouldContinue()
    {
        if ( closingServer )
        {
            closingConnections = false;
            closingServer = false;
            CloseServer();
            return false;
        }
        if ( closingConnections )
        {
            closingConnections = false;
            CloseConnections();
            return true;
        }
        return true;
    }
    private void CloseConnections()
    {
        ServerLog( "Closing Connections" );

        while ( clients.Count > 0 )
        {
            int i = clients.Count - 1;
            clients[ i ].tcpClient.Close();
            clients.RemoveAt( i );
        }
    }

    private void CloseServer()
    {
        ServerLog( "Closing Server" );

        CloseConnections();

        if( server != null )
        {
            server.Stop();
            server = null;
        }
    }

    public void TryCloseConnections()
    {
        closingConnections = true;
    }

    public void TryCloseServer()
    {
        closingServer = true;
    }

    public string toASCII( byte[] array )
    {
        return System.Text.Encoding.ASCII.GetString( array );
    }

    public string toHex( byte[] array )
    {
        return String.Join( String.Empty,
                Array.ConvertAll( array, x => x.ToString( "X2" ) ) );
    }
    private void ServerLog( string msg )
    {
        Debug.Log( "Server: " + msg );
    }
}
