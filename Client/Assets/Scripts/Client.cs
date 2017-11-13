using UnityEngine;

public class Client : MonoBehaviour {

  public short port = 9591;

  Listener listener;

  void Awake() {
    listener = new Listener(port);
    listener.Start();
  }

  void Update() {
    Target target = listener.Out;
    Debug.Log(target);
  }

  void OnDestroy() {
    listener.Stop();
    listener.Cleanup();
  }
}
