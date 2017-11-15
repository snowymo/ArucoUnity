using System.Collections.Generic;
using UnityEngine;

public class TrackedObject : MonoBehaviour {

  const int CAMERA_ID = 0; // Temporary

  public Client client;
  public int id;

  Target lastTarget;
  float deltaTime = 0;

  Holojam.Utility.AdaptiveSmoother smoother
    = new Holojam.Utility.AdaptiveSmoother();
  Vector3 lastPosition;
  Quaternion lastRotation;

  void Update() {
    // Get target from listener with camera and target IDs
    var pair = new KeyValuePair<int, int>(CAMERA_ID, id);
    Target target = client.listener.GetTarget(pair);

    deltaTime += Time.deltaTime;

    transform.position = smoother.Smooth(
      target.GetPosition(),
      ref lastPosition,
      deltaTime
    );

    transform.rotation = smoother.Smooth(
      target.GetRotation(),
      ref lastRotation,
      deltaTime
    );

    // Reset delta time if the data changed
    if (!target.Equals(lastTarget)) deltaTime = 0;

    lastTarget = target;
  }
}
