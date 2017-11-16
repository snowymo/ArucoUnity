using System;
using System.Net;
using UnityEngine;

public struct Target {

  public const int SIZE
    = 2 * sizeof(ushort)
    + 7 * sizeof(double);

  public ushort cam_id;
  public ushort target_id;

  public double pos_x;
  public double pos_y;
  public double pos_z;

  public double rot_x;
  public double rot_y;
  public double rot_z;
  public double rot_w;

  public static Target Deserialize(byte[] buffer) {
    Target target = new Target();
    int index = 0;

    target.cam_id = (ushort)IPAddress.NetworkToHostOrder(
      BitConverter.ToInt16(buffer, index)
    );

    index += sizeof(ushort);

    target.target_id = (ushort)IPAddress.NetworkToHostOrder(
      BitConverter.ToInt16(buffer, index)
    );

    index += sizeof(ushort);

    /* Position */

    target.pos_x = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    target.pos_y = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    target.pos_z = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    /* Rotation */

    target.rot_x = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    target.rot_y = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    target.rot_z = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    target.rot_w = BitConverter.ToDouble(buffer, index);

    return target;
  }

  public override string ToString() {
    return "cam_" + this.cam_id
      + " target_" + this.target_id
      + " pos (" + this.pos_x
      + ", " + this.pos_y
      + ", " + this.pos_z
      + ") rot (" + this.rot_x
      + ", " + this.rot_y
      + ", " + this.rot_z
      + ", " + this.rot_w + ")";
  }

  /* Convert to LHS */

  public Vector3 GetPosition() {
    return new Vector3(
      -1 * (float)this.pos_x,
      (float)this.pos_y,
           (float)this.pos_z
    );
  }

  public Quaternion GetRotation() {
    return new Quaternion(
      -1 * (float)this.rot_x,
           (float)this.rot_y,
      -1 * (float)this.rot_z,
        (float)this.rot_w
    );
  }
}
