#include "client.h"
#include "string/string.h"
#include "unistd.h"
#include <assert.h>
#include <stddef.h>
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
      return true;
    }
    if (n < 0)
      break;

    str slice = STR((char *)buf);

    ptrdiff_t index = STR_FIND(&slice, STR_LIT("Sec-WebSocket-Key: "));
    if (index == -1) {
      printf("websocket key not found\n");
      break;
    }

    str line = str_slice(&slice, index, slice.len);
    str_split_mut(&line, STR_LIT("\r\n"));
    line = str_trim(&line, STR_LIT(" "));
    assert(line.len > 0);

    str ws_key = str_split_mut(&line, STR_LIT(" "));
    assert(ws_key.len > 0);

    printf(SV_Fmt "\n", SV_Arg(ws_key));
  }

  return false;
}
