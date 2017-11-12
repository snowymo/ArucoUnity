#ifndef SENDER_H
#define SENDER_H

#include <cstdint>
#include <stddef.h>
#include <stdio.h>
#include <winsock2.h>

#define DEV_ADDR_0 "192.168.1.77"
#define DEV_ADDR_1 "192.168.1.78"
#define PORT 9591

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
};

int serialize(const target &data, char *buf);
int deserialize(char *buf, target &data);

struct sender {
  sockaddr_in dev0, dev1;
  WSADATA wsa;
  int sock;
  size_t devlen;
};

int initSender(sender &s);
int cleanupSender(const sender &s);
int sendData(const sender &s, const target &data);

#endif
