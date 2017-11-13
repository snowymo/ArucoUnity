using System;

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
