#ifndef SENDER_H
#define SENDER_H

#include <cstdint>
#include <stddef.h>
#include <stdio.h>

#ifndef _WIN32
# include "stubs.h"
# include <string.h>
#else
# include <winsock2.h>
#endif

#pragma comment(lib, "Ws2_32.lib")

#define DEV_ADDR_0 "172.16.42.90"
#define DEV_ADDR_1 "216.165.71.230"
#define DEV_PORT  9591
#define HOST_PORT 9592

#define WSAERR(msg) printf(msg ": %d\n", WSAGetLastError())

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

  target() {}
  target(uint16_t c, uint16_t t);
  target(uint16_t c, uint16_t t, double x, double y, double z, double rx, double ry, double rz, double rw);
};

int serialize(const target &data, char *buf);
int deserialize(char *buf, target &data);

struct sender {
  sockaddr_in host, dev0, dev1;
  WSADATA wsa;
  SOCKET sock;
  size_t devlen;
};

int initSender(sender &s);
int cleanupSender(const sender &s);
int sendData(const sender &s, const target &data);

#endif
