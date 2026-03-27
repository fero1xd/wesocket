#include "server.h"
#include <stdio.h>

#include "client.c"
#include "server.c"

int main(void) {
  weserver_t w = {.config = {.addr = "0.0.0.0", .port = 3000}};

  wesocket_run(w);

  return 0;
};
