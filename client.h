#pragma once

#include "consts.h"

struct WeClient {
  socket_fd fd;
};

typedef struct WeClient weclient_t;

bool weclient_run(weclient_t);
