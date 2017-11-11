#include "sender.h"

int main() {
  sender s = sender();
  initSender(s);

  target t = target();
  sendData(s, t);

  cleanupSender(s);

  return 0;
}
