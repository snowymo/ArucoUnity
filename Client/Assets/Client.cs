using UnityEngine;

public class Client : MonoBehaviour {

  public short port = 9591;

  Listener listener;

  void Awake() {
    listener = new Listener(port);
    listener.Start();
  }
}
