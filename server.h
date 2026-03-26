#pragma once

#include "ints.h"

struct WeConfig {
  u16 port;
  char *addr;
};

typedef struct WeConfig weconfig_t;

typedef int socket_fd;

struct WeServer {
  weconfig_t config;
  socket_fd fd;
};

typedef struct WeServer weserver_t;

void wesocket_run(weserver_t w);
