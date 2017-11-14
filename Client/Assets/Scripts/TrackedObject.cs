using System.Collections.Generic;
using UnityEngine;

public class TrackedObject : MonoBehaviour {

  const int CAMERA_ID = 0; // Temporary

  public Client client;
  public int id;

  void Update() {
    var pair = new KeyValuePair<int, int>(CAMERA_ID, id);
    Target target = client.listener.GetTarget(pair);

    transform.position = new Vector3(
      (float)target.pos_x,
      (float)target.pos_y,
      (float)target.pos_z
    );

    transform.rotation = new Quaternion(
      (float)target.rot_x,
      (float)target.rot_y,
      (float)target.rot_z,
      (float)target.rot_w
    );
  }
}
