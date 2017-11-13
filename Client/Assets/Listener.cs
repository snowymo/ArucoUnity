using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

public class Listener {

  Socket sock = null;
  IPEndPoint end = null;
  Thread thread;

  public Listener(short port) {
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

    /* Listen */

    var start = new ThreadStart(
      () => {
        while (true) Update();
      }
    );

    thread = new Thread(start);
  }

  public void Start() {
    thread.Start();
  }

  void Update() {
    byte[] buffer = new byte[Target.SIZE];

    try {
      // Get buffer (blocking)
      if (sock.Receive(buffer) == 0) return;
    } catch (Exception e) {
      Debug.LogWarning(e);
      return;
    }

    Target target = Target.Deserialize(buffer);

    Debug.Log(target);
  }
}
