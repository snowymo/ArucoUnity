#include "target.h"

int serialize(const target &data, char *buf) {
	uint16_t *ids = (uint16_t*)buf;

	*ids = data.cam_id;    ids++;
	*ids = data.target_id; ids++;

	double *tra = (double*)ids;

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

	double *tra = (double*)ids;

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

	char buf[target::SIZE];
	serialize(data, buf);

	/* Unicast to devices */

	return 1;
}

target::target(uint16_t c, uint16_t t, double x, double y, double z, double rx, double ry, double rz, double rw)
	:cam_id(c), target_id(t), pos_x(x), pos_y(y), pos_z(z), rot_x(rx), rot_y(ry), rot_z(rz), rot_w(rw)
{
	
}
