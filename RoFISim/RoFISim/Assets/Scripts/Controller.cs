using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

using MessageQueue = System.Collections.Concurrent.ConcurrentQueue< System.Tuple< int, System.Collections.Generic.List< byte > > >;

public class Controller : MonoBehaviour
{
    private MessageQueue received = new MessageQueue();
    private MessageQueue send = new MessageQueue();

    private TCPServer server;

    void Start()
    {
        server = new TCPServer( received, send );
        server.StartServer();
    }

    void Update()
    {
        Tuple< int, List< byte > > item;
        if ( received.TryDequeue( out item ) ) {
            // TODO
            send.Enqueue( item );
        }
    }
}
