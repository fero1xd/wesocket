#include "client.h"
#include "allocator/arena.h"
#include "data_frame.h"
#include "handshake.h"
#include "string/string.h"
#include "unistd.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

bool weclient_run(weclient_t client) {
  u8 buf[BUFFER_SIZE];
  Arena arena = {0};
  Arena frame_arena = {0};
  data_frame_t *f = data_frame_new(&frame_arena);
  int read_offset = 0;

  while (true) {
    if (!client.handshake)
      memset(buf, 0, BUFFER_SIZE);
    assert(read_offset < BUFFER_SIZE);
    int n = read(client.fd, buf + read_offset, BUFFER_SIZE - read_offset);
    if (n == 0) {
      printf("EOF client: %d\n", client.fd);
      arena_free(&arena);
      arena_free(&frame_arena);
      return true;
    }
    if (n < 0)
      break;

    str *slice = STR_WITH_LEN_LIT((char *)buf, n);

    if (!client.handshake) {
      ptrdiff_t index = STR_FIND(slice, STR_LIT("Sec-WebSocket-Key: "));
      if (index == -1) {
        printf("websocket key not found\n");
        break;
      }

      str line = str_slice(slice, index, slice->len);
      line = str_slice(&line, 0, STR_FIND(&line, STR_LIT("\r\n")));
      line = str_trim(&line, STR_LIT(" "));
      assert(line.len > 0);

      str ws_key = str_split_mut(&line, STR_LIT(" "));
      assert(ws_key.len > 0);
      str hash = make_handshake_accept_key(&arena, &ws_key);

      str *res = STR_BUF(400);
      res->len = snprintf(res->ptr, 400,
                          "HTTP/1.1 101 Switching Protocols\r\n"
                          "Upgrade: websocket\r\n"
                          "Connection: Upgrade\r\n"
                          "Sec-WebSocket-Accept: %.*s\r\n\r\n",
                          (int)hash.len, hash.ptr);

      write(client.fd, res->ptr, res->len);

      printf(SV_Fmt, SV_Arg(*res));
      printf("Written: %ld\n", res->len);

      client.handshake = true;
    } else {
      read_offset += n;
      printf("Read: %ld bytes\n", slice->len);

      if (f->state == DONE) {
        f = data_frame_new(&frame_arena);
      }

      str *slice = STR_WITH_LEN_LIT((char *)buf, read_offset);
      size_t read = parse_data_frame(&frame_arena, f, slice);

      if (read > 0) {
        memmove(buf, buf + read, read_offset - read);
        read_offset -= read;
      }

      printf("Read: %ld bytes as packet, status: %d\n", read, f->state);

      if (f->state == DONE && f->header->fin) {
        printf("Fin: %d\n", f->header->fin);
        printf("len: %d\n", f->header->payload_len);
        printf("ex_len: %lld\n", f->header->extended_payload_len);
        printf("read: %lld\n", f->bytes_read);
        printf("Received: %.*s\n", (int)f->header->payload_len,
               (char *)f->payload);

        arena_free(&frame_arena);
        f = data_frame_new(&frame_arena);
      }
    }
  }

  arena_free(&arena);
  arena_free(&frame_arena);
  return false;
}
