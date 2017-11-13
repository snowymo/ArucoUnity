using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

public class Listener {

  public Target Out {
    get {
      lock (sync) { return lastValidData; }
    }
  }

  Socket sock = null;
  IPEndPoint end = null;
  byte[] buffer;

  Thread thread;
  volatile bool running;
  object sync;
  Target lastValidData;

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

    buffer = new byte[Target.SIZE];
    running = false;
    sync = new object();

    var start = new ThreadStart(
      () => {
        Debug.Log("Listener thread started");
        while (running) Update();
        Debug.Log("Listener thread stopped");
      }
    );

    thread = new Thread(start);
  }

  public void Start() {
    if (running) return;
    running = true;

    thread.Start();
  }

  void Update() {
    try {
      // Get buffer (blocking)
      if (sock.Receive(buffer) == 0) return;
    } catch (Exception e) {
      Debug.LogWarning(e);
      return;
    }

    Target target = Target.Deserialize(buffer);
    lock(sync) { lastValidData = target; }
  }

  public void Stop() {
    running = false;
    thread.Join();
  }
}
