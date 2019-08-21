using UnityEngine;
using UnityEngine.Networking;

public class Server : MonoBehaviour {
    private const int maxConnections = 1;
    private const string serverIp = "127.0.0.1";
    private const int serverPort = 9000;
    private const int bufferSize = 1024;

    private int reliableChannelId;
    private int unreliableChannelId;

    private int hostId;

    private byte[] buffer = new byte[bufferSize];

    void Start () {
        GlobalConfig config = new GlobalConfig ();
        NetworkTransport.Init (config);

        ConnectionConfig cc = new ConnectionConfig ();
        reliableChannelId = cc.AddChannel (QosType.Reliable);
        unreliableChannelId = cc.AddChannel (QosType.Unreliable);

        HostTopology topo = new HostTopology (cc, maxConnections);

        // Adding hosts
        hostId = NetworkTransport.AddHost (topo, serverPort);

        Debug.Log("Started server on port " + serverPort);

    }

    void Update () {

        int outHostId, outConnectionId, outChannelId;
        int receivedSize;
        byte error;

        NetworkEventType e = NetworkTransport.Receive (out outHostId, out outConnectionId, out outChannelId, buffer, buffer.Length, out receivedSize, out error);

        if (e == NetworkEventType.Nothing) {
            return;
        }

        switch (e) {
            case NetworkEventType.Nothing:
                return;
            case NetworkEventType.DataEvent:
                Debug.Log("Data from " + outConnectionId + " through the channel " + outChannelId + " ::: Message ::: " + buffer);
                break;
            case NetworkEventType.BroadcastEvent:
                Debug.Log("Broadcast from " + outConnectionId + " through the channel " + outChannelId);
                break;
            case NetworkEventType.ConnectEvent:
                Debug.Log("Connection from " + outConnectionId + " through the channel " + outChannelId);
                break;
            case NetworkEventType.DisconnectEvent:
                Debug.Log("Disconnected from " + outConnectionId + " through the channel " + outChannelId);
                break;
        }

    }

}