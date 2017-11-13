using System;
using System.Net;
using System.Net.Sockets;
using UnityEngine;

public class Client : MonoBehaviour {

  public short port = 9591;

  Socket sock = null;
  IPEndPoint end = null;

  void Awake() {
    /* Create */

    try {
      sock = new Socket(
        AddressFamily.InterNetwork,
        SocketType.Dgram,
        ProtocolType.Udp
      );
    } catch (Exception e) {
      Debug.LogWarning(e);
      return;
    }

    Debug.Log("Created socket with handle " + sock.Handle);

    /* Bind */

    try {
      end = new IPEndPoint(
        IPAddress.Any,
        port
      );
    } catch (Exception e) {
      Debug.LogWarning(e);
      return;
    }

    try {
      sock.Bind(end);
    } catch (Exception e) {
      Debug.LogWarning(e);
      return;
    }

    Debug.Log("Bound socket to " + end);
  }
}
