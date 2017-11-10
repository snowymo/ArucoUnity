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

int serialize(target &data, char *buf) {
  uint16_t *ids = (uint16_t*)buf;

  *ids = data.cam_id;    ids++;
  *ids = data.target_id; ids++;

  double *tra = (double*)buf;

  *tra = data.pos_x; tra++;
  *tra = data.pos_y; tra++;
  *tra = data.pos_z; tra++;

  *tra = data.rot_x; tra++;
  *tra = data.rot_y; tra++;
  *tra = data.rot_z; tra++;
  *tra = data.rot_w; tra++;

  return 0;
}

int deserialize(char *buf, target &data) {
  uint16_t *ids = (uint16_t*)buf;

  data.cam_id = *ids;    ids++;
  data.target_id = *ids; ids++;

  double *tra = (double*)buf;

  data.pos_x = *tra; tra++;
  data.pos_y = *tra; tra++;
  data.pos_z = *tra; tra++;

  data.rot_x = *tra; tra++;
  data.rot_y = *tra; tra++;
  data.rot_z = *tra; tra++;
  data.rot_w = *tra; tra++;

  return 0;
}

int sendData(const target &data) {

  /* Build packet */

  /* Unicast to devices */

  return 1;
}
