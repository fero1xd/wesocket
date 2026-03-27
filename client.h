#pragma once

#include "consts.h"

struct WeClient {
  socket_fd fd;
  bool handshake;
};

typedef struct WeClient weclient_t;

bool weclient_run(weclient_t);
