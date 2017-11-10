#include <cstdint>
#include <stddef.h>
#include <winsock2.h>

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
  static sockaddr_in dev0, dev1;
  static WSADATA wsa;
  static int sock;
};

int initSender() {
  if (WSAStartup(MAKEWORD(2, 2), &sender::wsa) != 0) {
    printf("Error during init: %d\n", WSAGetLastError());
    return 1;
  }

  sender::sock = socket(
    AF_INET,
    SOCK_DGRAM,
    IPPROTO_UDP
  );

  if (sender::sock == SOCKET_ERROR) {
    printf("Socket error: %d\n", WSAGetLastError());
    return 1;
  }

  memset((char*)&sender::dev0, 0, sizeof(sender::dev0));
  sender::dev0.sin_family = AF_INET;
  sender::dev0.sin_port = htons(PORT);
  sender::dev0.sin_addr.S_un.S_addr = inet_addr(DEV_ADDR_0);

  memset((char*)&sender::dev1, 0, sizeof(sender::dev1));
  sender::dev1.sin_family = AF_INET;
  sender::dev1.sin_port = htons(PORT);
  sender::dev1.sin_addr.S_un.S_addr = inet_addr(DEV_ADDR_1);

  return 0;
}

int cleanupSender() {
  closesocket(sender::sock);
  WSACleanup();

  return 0;
}

int sendData(const target &data) {
  /* Build packet */

  char buf[target::SIZE];
  serialize(data, buf);

  /* Unicast to devices */

  int err;

  err = sendto(
    sock,
    buf,
    target::SIZE,
    0,
    (struct sockaddr*)&sender::dev0,
    sizeof(sender::dev0)
  );

  if (err == SOCKET_ERR) {
    printf("Error during send: %d\n", WSAGetLastError());
    return 1;
  }

  err = sendto(
    sock,
    buf,
    target::SIZE,
    0,
    (struct sockaddr*)&sender::dev1,
    sizeof(sender::dev1)
  );

  if (err == SOCKET_ERR) {
    printf("Error during send: %d\n", WSAGetLastError());
    return 1;
  }

  return 0;
}
