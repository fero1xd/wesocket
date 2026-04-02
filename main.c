#include "server.h"
#include <stdio.h>

#include "client.c"
#include "data_frame.c"
#include "handshake.c"
#include "server.c"
#include "string/base64.c"

#define ARENA_IMPLEMENTATION
#include "allocator/arena.h"

int main(void) {
  weserver_t w = {.config = {.addr = "0.0.0.0", .port = 3000}};
  wesocket_run(w);

  return 0;
};
