using UnityEngine;

public class Client : MonoBehaviour {

  public short port = 9591;

  public Listener listener;

  void Awake() {
    listener = new Listener(port);
    listener.Start();
  }

  void OnDestroy() {
    listener.Stop();
    listener.Cleanup();
  }
}
