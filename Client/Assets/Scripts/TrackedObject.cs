using System.Collections.Generic;
using UnityEngine;

public class TrackedObject : MonoBehaviour {

  const int CAMERA_ID = 0; // Temporary

  public Client client;
  public int id;

  Target lastTarget;
  float deltaTime = 0;

  void Update() {
    // Get target from listener with camera and target IDs
    var pair = new KeyValuePair<int, int>(CAMERA_ID, id);
    Target target = client.listener.GetTarget(pair);

    deltaTime += Time.deltaTime;

    transform.position = target.GetPosition();
    transform.rotation = target.GetRotation();

    // Reset delta time if the data changed
    if (!target.Equals(lastTarget)) deltaTime = 0;

    lastTarget = target;
  }
}
