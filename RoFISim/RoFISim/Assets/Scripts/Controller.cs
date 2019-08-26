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

    private List< RoFIModule > modules = new List< RoFIModule >();
    private BiMap< int, RoFIModule > clientModuleBiMap = new BiMap< int, RoFIModule >();

    void Start()
    {
        server = new TCPServer( received, send );
        server.StartServer();
    }

    void Update()
    {
        Tuple< int, List< byte > > item;
        while ( received.TryDequeue( out item ) )
        {
            RoFIModule module = clientModuleBiMap.Forward[ item.Item1 ];
            if ( module == null )
            {
                module = AddClient( item.Item1 );
                if ( module == null )
                {
                    continue;
                }
            }

            Rofi.Command command = Rofi.Command.Parser.ParseFrom( item.Item2.ToArray() );
            command.execute( this, module );
        }
    }

    public RoFIModule AddClient( int clientId )
    {
        foreach ( RoFIModule module in modules )
        {
            if ( !clientModuleBiMap.Reverse.ContainsKey( module ) )
            {
                clientModuleBiMap.Add( clientId, module );
                return module;
            }
        }

        Debug.Log( "More clients connected than RoFI Modules." );
        return null;
    }

    public void SendMessage( RoFIModule fromModule, Command command )
    {
        int? clientId = clientModuleBiMap.Reverse[ fromModule ];
        if ( clientId == null )
        {
            return;
        }
        byte[] cmdMsg = new byte[ command.CalculateSize() ];
        command.WriteTo( CodedOutputStream( cmdMsg ) );
        send.Enqueue( Tuple.Create( clientId.Value, List< byte >( cmdMsg ) ) );
    }
}
