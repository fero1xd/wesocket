#include "data_frame.c"
#include "data_frame.h"

#include "frame_builder.h"
#include <assert.h>

#include "frame_builder.c"
#include "string/string.h"

#define ARENA_IMPLEMENTATION
#include "allocator/arena.h"

Arena a = {0};
Arena *ctx = &a;

int main(void) {
  u8 packet[] = {
      // FIN=0, RSV1-3=0, opcode=0x1 (text)
      0b10000001,
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
  };

  size_t frame_size =
      frame_calculate_size((calculate_opts){.mask = true, .payload_len = 5});

  assert(frame_size == sizeof(packet));
  printf("Frame Size: %ld\n", frame_size);

  str *frame = STR_BUF_A(ctx, frame_size);

  str *message = STR_LIT("hello");
  str *payload = STR_BUF_A(ctx, message->len);

  mask_frame_payload(message, &packet[2], payload);

  size_t written = frame_write(
      &(frame_write_opts){
          .fin = true,
          .is_masked = true,
          .mask = &packet[2],
          .opcode = TEXT,
          .payload = payload,

      },
      frame);

  printf("Frame written: %ld\n", written);

  assert(written == frame_size);

  for (size_t i = 0; i < frame_size; i++) {
    assert((u8)frame->ptr[i] == packet[i]);
  }

  arena_free(ctx);
  return 0;
}
