#include "sender.h"

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
