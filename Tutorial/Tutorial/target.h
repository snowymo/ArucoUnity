#ifndef _TARGET_H
#define _TARGET_H

#include <cstdint>
#include <stddef.h>

#define DEV_ADDR_0 192.168.1.77
#define DEV_ADDR_1 192.168.1.78

struct target {
	static const size_t SIZE
		= 2 * sizeof(uint16_t)
		+ 7 * sizeof(double);

	uint16_t cam_id;
	uint16_t target_id;

	double pos_x;
	double pos_y;
	double pos_z;

	double rot_x;
	double rot_y;
	double rot_z;
	double rot_w;

	target(uint16_t c, uint16_t t, double x, double y, double z, double rx, double ry, double rz, double rw);
};

int serialize(const target &data, char *buf);

int deserialize(char *buf, target &data);

int sendData(const target &data);

#endif // !_TARGET_H