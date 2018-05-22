#ifndef SENDER_H
#define SENDER_H

#include <cstdint>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#ifndef _WIN32
# include "stubs.h"
# include <string.h>
#else
# include <winsock2.h>
#endif

#pragma comment(lib, "Ws2_32.lib")

#define HOST_ADDR "127.0.0.1"
#define DEV_PORT  20011
#define HOST_PORT 20010

#define WSAERR(msg) printf(msg ": %d\n", WSAGetLastError())

struct target {
  static const size_t SIZE
    = 2 * sizeof(int)
    + 7 * sizeof(double);

  int cam_id;
  int target_id;

  double pos_x;
  double pos_y;
  double pos_z;

  double rot_x;
  double rot_y;
  double rot_z;
  double rot_w;

  target() {}
  target(int c, int t);
  target(int c, int t, double x, double y, double z, double rx, double ry, double rz, double rw);
};

int serialize(const target &data, char *buf);
int deserialize(char *buf, target &data);

struct sender {
  sockaddr_in dev0;
  std::string send_ip;
  WSADATA wsa;
  SOCKET sock;
  size_t devlen;
};

int initSender(sender &s);
int cleanupSender(const sender &s);
int sendData(const sender &s, const target &data);

#endif
