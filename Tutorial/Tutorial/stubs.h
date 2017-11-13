#ifndef STUBS_H
#define STUBS_H

#define SOCKET int

#define SOCKET_ERROR -1
#define AF_INET       0
#define SOCK_DGRAM    0
#define IPPROTO_UDP   0
#define INADDR_ANY    0

#define MAKEWORD(a, b) 0

#define WSAStartup(a, b)  0
#define WSAGetLastError() 0
#define WSACleanup()      ;

#define inet_addr(s) 0
#define htons(s)     s
#define nstoh(s)     s

#define socket(a, b, c)                0
#define bind(a, b, c)                  0
#define sendto(s, buf, len, o, out, l) 0
#define closesocket(s)                 ;

typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef unsigned char  uchar;

struct WSADATA {};

struct sockaddr {
  ushort sa_family;
  char   sa_data[14];
};

struct in_addr {
  union {
    struct {
      uchar s_b1, s_b2, s_b3, s_b4;
    } s_un_b;

    struct {
      ushort s_w1, s_w2;
    } s_un_w;

    ulong S_addr;

  } S_un;

  ulong s_addr;
};

struct sockaddr_in {
  short   sin_family;
  ushort  sin_port;
  in_addr sin_addr;
  char    sin_zero[8];
};

#endif
