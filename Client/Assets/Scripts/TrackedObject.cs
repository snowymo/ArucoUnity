using System.Collections.Generic;
using UnityEngine;

public class TrackedObject : MonoBehaviour {

  const int CAMERA_ID = 0; // Temporary

  public Client client;
  public int id;

  void Update() {
    var pair = new KeyValuePair<int, int>(CAMERA_ID, id);
    Target target = client.listener.GetTarget(pair);

    transform.position = target.GetPosition();
    transform.rotation = target.GetRotation();
  }
}
