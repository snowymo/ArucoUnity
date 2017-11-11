#include <cstdint>
#include <stddef.h>
#include <stdio.h>
//#include <winsock2.h>

#define DEV_ADDR_0 "192.168.1.77"
#define DEV_ADDR_1 "192.168.1.78"
#define PORT 9591

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

struct sender {
  sockaddr_in dev0, dev1;
  WSADATA wsa;
  int sock;
  size_t devlen;
};

int initSender(sender &s) {
  if (WSAStartup(MAKEWORD(2, 2), &s.wsa) != 0) {
    printf("Error during init: %d\n", WSAGetLastError());
    return 1;
  }

  s.sock = socket(
    AF_INET,
    SOCK_DGRAM,
    IPPROTO_UDP
  );

  if (s.sock == SOCKET_ERROR) {
    printf("Socket error: %d\n", WSAGetLastError());
    return 1;
  }

  memset((char*)&s.dev0, 0, sizeof(s.dev0));
  s.dev0.sin_family = AF_INET;
  s.dev0.sin_port = htons(PORT);
  s.dev0.sin_addr.S_un.S_addr = inet_addr(DEV_ADDR_0);

  memset((char*)&s.dev1, 0, sizeof(s.dev1));
  s.dev1.sin_family = AF_INET;
  s.dev1.sin_port = htons(PORT);
  s.dev1.sin_addr.S_un.S_addr = inet_addr(DEV_ADDR_1);

  s.devlen = sizeof(s.dev0);

  return 0;
}

int cleanupSender(const sender &s) {
  closesocket(s.sock);
  WSACleanup();

  return 0;
}

int sendData(const sender &s, const target &data) {
  /* Build packet */

  char buf[target::SIZE];
  serialize(data, buf);

  /* Unicast to devices */

  int err;

  err = sendto(
    s.sock,
    buf,
    target::SIZE,
    0,
    (struct sockaddr*)&s.dev0,
    s.devlen
  );

  if (err == SOCKET_ERROR) {
    printf("Error during send: %d\n", WSAGetLastError());
    return 1;
  }

  err = sendto(
    s.sock,
    buf,
    target::SIZE,
    0,
    (struct sockaddr*)&s.dev1,
    s.devlen
  );

  if (err == SOCKET_ERROR) {
    printf("Error during send: %d\n", WSAGetLastError());
    return 1;
  }

  return 0;
}
