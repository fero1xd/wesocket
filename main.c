#include "server.h"
#include <stdio.h>

#include "server.c"

int main(void) {
  weserver_t w = {.config = {.addr = "127.0.0.1", .port = 3000}};

  wesocket_run(w);

  return 0;
};
