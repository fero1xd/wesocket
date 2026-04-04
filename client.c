#include "client.h"
#include "allocator/arena.h"
#include "handshake.h"
#include "message.h"
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

  int read_offset = 0;
  message_t m = new_message();

  while (true) {
    if (!client.handshake)
      memset(buf, 0, BUFFER_SIZE);

    assert(read_offset < BUFFER_SIZE);
    int n = read(client.fd, buf + read_offset, (BUFFER_SIZE - read_offset));

    if (n == 0) {
      printf("EOF client: %d\n", client.fd);
      arena_free(&arena);
      arena_free(&frame_arena);
      return true;
    }
    if (n < 0)
      break;

    read_offset += n;
    str slice = STR_WITH_LEN((char *)buf, read_offset);

    if (!client.handshake) {
      ptrdiff_t index = STR_FIND(&slice, STR_LIT("Sec-WebSocket-Key: "));
      if (index == -1) {
        printf("websocket key not found\n");
        break;
      }

      str line = STR_SLICE(&slice, index);
      line = STR_SLICE(&line, 0, STR_FIND(&line, STR_LIT("\r\n")));
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
      read_offset -= n;
    } else {
      printf("Read: %ld bytes\n", slice.len);
      if (m.done) {
        m = new_message();
      }

      size_t read = -1;
      while (!m.done && slice.len > 0 && read != 0) {
        read = parse_message(&frame_arena, &m, &slice);
        printf("Read: %ld bytes as message, state: %d\n", read,
               m.current_frame->state);

        if (read > 0) {
          memmove(buf, buf + read, read_offset - read);
          read_offset -= read;
          slice = STR_SLICE(&slice, read);
        }

        if (m.pending_control_frame && m.current_frame->state == DONE) {
          printf("Pending control frame: %x\n",
                 m.current_frame->header->opcode);
          printf("Data length: %lld\n\n", m.current_frame->bytes_read);
        }

        if (m.done) {
          printf("read_offset: %d, bytes_read: %ld\n", read_offset,
                 m.bytes_read);
          printf("Received: %.*s\n", (int)m.payload.len, (char *)m.payload.ptr);

          read_offset = 0;
          arena_free(&frame_arena);
        }
      }
    }
  }

  arena_free(&arena);
  arena_free(&frame_arena);
  return false;
}
