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

  public static Target Deserialize(byte[] buffer) {
    Target target = new Target();
    int index = 0;

    target.cam_id = BitConverter.ToUInt16(buffer, index);
    index += sizeof(ushort);

    target.cam_id = BitConverter.ToUInt16(buffer, index);
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

    return target;
  }
}
