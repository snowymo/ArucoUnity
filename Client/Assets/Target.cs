using System;

public struct Target {

  public const int SIZE
    = 2 * sizeof(ushort)
    + 7 * sizeof(double);

  ushort cam_id;
  ushort target_id;

  double pos_x;
  double pos_y;
  double pos_z;

  double rot_x;
  double rot_y;
  double rot_z;
  double rot_w;

  public void Deserialize(byte[] buffer) {
    int index = 0;

    this.cam_id = BitConverter.ToUInt16(buffer, index);
    index += sizeof(ushort);

    this.cam_id = BitConverter.ToUInt16(buffer, index);
    index += sizeof(ushort);

    /* Position */

    this.pos_x = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    this.pos_y = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    this.pos_z = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    /* Rotation */

    this.rot_x = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    this.rot_y = BitConverter.ToDouble(buffer, index);
    index += sizeof(double);

    this.rot_z = BitConverter.ToDouble(buffer, index);
  }
}
