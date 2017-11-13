public struct Target {

  const int SIZE
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
}
