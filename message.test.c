#include "message.h"
#include "data_frame.h"

#define ARENA_IMPLEMENTATION
#include "allocator/arena.h"
#include "string/string.h"
#include <stdio.h>

#include "data_frame.c"
#include "message.c"

Arena arena = {0};
Arena *ctx = &arena;

int main(void) {
  u8 packet[] = {
      // FIN=0, RSV1-3=0, opcode=0x1 (text)
      0b00000001,
      // MASK=1, payload length=5
      0b10000101,
      // 4-byte mask
      1,
      2,
      3,
      4,
      // 5-byte payload
      105,
      103,
      111,
      104,
      110,

      // FIN=0, RSV1-3=0, opcode=0x1 (text)
      0b00000000,
      // MASK=1, payload length=5
      0b10000001,
      // 4-byte mask
      1,
      2,
      3,
      4,
      // 1-byte payload
      33,

      // FIN=0, RSV1-3=0, opcode=0x1 (cont)
      0b10000000,
      // MASK=1, payload length=5
      0b10000101,
      // 4-byte mask
      1,
      2,
      3,
      4,
      // 5-byte payload
      118,
      109,
      113,
      104,
      101,
  };

  str slice = STR_WITH_LEN((char *)packet, sizeof(packet));
  message_t message = new_message();

  while (!message.done && !message.pending_control_frame) {
    size_t read = parse_message(ctx, &message, &slice);
    if (read == 0)
      break;

    slice = STR_SLICE(&slice, read);
  }

  if (!message.done && !message.pending_control_frame) {
    printf("Not enough data to parse a packet\n");
    return 1;
  }

  printf("opcode: %d\n", GET_OPCODE(message));
  printf("frame count: %ld\n", message.frames);
  printf("payload length: %ld\n", GET_PAYLOAD_LEN(message));
  printf("Payload: %.*s\n", (int)message.payload.len,
         (char *)message.payload.ptr);

  arena_free(ctx);
  return 0;
}
