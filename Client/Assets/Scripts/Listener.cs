using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Collections.Generic;
using UnityEngine;

public class Listener {

  const int TIMEOUT = 1000; // ms

  Socket sock = null;
  IPEndPoint end = null;
  byte[] buffer;

  Thread thread;
  volatile bool running;
  object sync;

  Dictionary<KeyValuePair<int, int>, Target> data;

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

    sock.ReceiveTimeout = TIMEOUT;

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
    data = new Dictionary<KeyValuePair<int, int>, Target>();

    var start = new ThreadStart(
      () => {
        Debug.Log("Listener thread started");
        while (running) Update();
        Debug.Log("Listener thread stopped");
      }
    );

    thread = new Thread(start);
  }

  public Target GetTarget(KeyValuePair<int, int> pair) {
    Target target = new Target();
    lock(sync) { data.TryGetValue(pair, out target); }
    return target;
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
    } catch (SocketException e) {
      if (e.ErrorCode == 10035 || e.ErrorCode == 10060) {
        Debug.LogWarning("Listener timeout: " + e);
      }
    } catch (Exception e) {
      Debug.LogWarning(e);
      return;
    }

    Target target = Target.Deserialize(buffer);
    var pair = new KeyValuePair<int, int>(target.cam_id, target.target_id);
    lock(sync) { data[pair] = target; }
  }

  public void Stop() {
    running = false;
    thread.Join();
  }

  public void Cleanup() {
    sock.Close();
  }
}
