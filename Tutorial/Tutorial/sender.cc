#include <cstdint>

#define DEV_ADDR_0 192.168.1.77
#define DEV_ADDR_1 192.168.1.78

struct target {
  uint16_t cam_id;
  uint16_t target_id;
  double pos_x;
  double pos_y;
  double pos_z;
  double rot_x;
  double rot_y;
  double rot_z;
  double rot_w;
};

int sendData(const target &data) {

  /* Build packet */

  /* Unicast to devices */

  return 1;
}
