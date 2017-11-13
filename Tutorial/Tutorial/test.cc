#include "sender.h"

int main() {
  sender s = sender();
  initSender(s);

  target t = target();

  while (true) {
    sendData(s, t);
  }

  cleanupSender(s);

  return 0;
}
