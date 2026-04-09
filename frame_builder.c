#include "frame_builder.h"
#include "data_frame.h"
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>

size_t frame_calculate_size(calculate_opts opts) {
  u8 extra_payload_bytes = opts.payload_len <= 125     ? 0
                           : (opts.payload_len == 126) ? 2
                                                       : 8;

  u8 mask_bytes = opts.mask ? 4 : 0;

  return HEADER_SIZE + extra_payload_bytes + mask_bytes + opts.payload_len;
}

size_t frame_write(frame_write_opts *opts, str *out) {
  size_t payload_len = opts->payload->len;
  size_t size = frame_calculate_size(
      (calculate_opts){.mask = opts->is_masked, .payload_len = payload_len});

  assert(out->len >= size);

  u8 *frame = (u8 *)out->ptr;

  frame[0] = opts->opcode;

  if (opts->fin) {
    frame[0] |= FIN_MASK;
  }

  if (opts->is_masked) {
    frame[1] = MASKING_MASK;
  }

  size_t offset = 2;

  if (payload_len < 126) {
    frame[1] |= payload_len;
  } else if (payload_len <= 0xFFFF) {
    frame[1] |= 126;
    u16 p = htons(payload_len);
    memcpy(&frame[2], &p, 2);
    offset = 4;
  } else {
    frame[1] = 127;
    u64 p = htonll(payload_len);
    memcpy(&frame[2], &p, 8);
    offset = 10;
  }

  if (opts->is_masked) {
    memcpy(&frame[offset], opts->mask, 4);
    offset += 4;
  }

  // if (opts->do_mask) {
  //   u8 *dest = &frame[offset];
  //   for (u64 i = 0; i < payload_len; i++) {
  //     dest[i] = ((u8)opts->payload->ptr[i]) ^ opts->mask[i % 4];
  //   }
  // } else {
  memcpy(&frame[offset], opts->payload->ptr, opts->payload->len);
  offset += payload_len;

  return offset;
}
