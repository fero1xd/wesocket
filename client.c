#include "client.h"
#include "unistd.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

bool weclient_run(weclient_t client) {
  u8 buf[BUFFER_SIZE];

  while (true) {
    memset(buf, 0, BUFFER_SIZE);
    int n = read(client.fd, buf, BUFFER_SIZE);
    if (n == 0) {
      printf("EOF client: %d\n", client.fd);
      break;
    }
    if (n < 0)
      break;

    printf("%s\n", buf);
  }

  return false;
}
