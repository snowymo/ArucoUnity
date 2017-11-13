#include "sender.h"

target::target(uint16_t c, uint16_t t) : cam_id(c), target_id(t)
{
}

target::target(uint16_t c, uint16_t t, double x, double y, double z, double rx, double ry, double rz, double rw)
	: cam_id(c), target_id(t), pos_x(x), pos_y(y), pos_z(z), rot_x(rx), rot_y(ry), rot_z(rz), rot_w(rw)
{

}


int serialize(const target &data, char *buf) {
  uint16_t *ids = (uint16_t*)buf;

  *ids = htons(data.cam_id);    ids++;
  *ids = htons(data.target_id); ids++;

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

  data.cam_id = ntohs(*ids);    ids++;
  data.target_id = ntohs(*ids); ids++;

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
    WSAERR("Error during init");
    return 1;
  }
 
  s.sock = socket(
    AF_INET,
    SOCK_DGRAM,
    IPPROTO_UDP
  );

  if (s.sock == SOCKET_ERROR) {
    WSAERR("Socket error");
    return 1;
  }

  s.host.sin_family = AF_INET;
  s.host.sin_port = htons(HOST_PORT);
  s.host.sin_addr.s_addr = INADDR_ANY;

  int err;

  err = bind(
    s.sock,
    (sockaddr*)&s.host,
    sizeof(s.host)
  );

  if (err == SOCKET_ERROR) {
    WSAERR("Bind error");
  }

  s.devlen = sizeof(s.dev0);

  memset((char*)&s.dev0, 0, s.devlen);

  s.dev0.sin_family = AF_INET;
  s.dev0.sin_port = htons(DEV_PORT);
  s.dev0.sin_addr.S_un.S_addr = inet_addr(DEV_ADDR_0);

  memset((char*)&s.dev1, 0, s.devlen);

  s.dev1.sin_family = AF_INET;
  s.dev1.sin_port = htons(DEV_PORT);
  s.dev1.sin_addr.S_un.S_addr = inet_addr(DEV_ADDR_1);

  printf("Sender initialized with socket %d\n", s.sock);

  return 0;
}

int cleanupSender(const sender &s) {
  closesocket(s.sock);
  WSACleanup();

  printf("Sender cleaned up\n");

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
    WSAERR("Error during send");
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
    WSAERR("Error during send");
    return 1;
  }

  return 0;
}
