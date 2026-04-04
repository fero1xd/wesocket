#include "data_frame.h"
#include "string/string.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MIN_HEADER_SIZE 1
#define MIN_PAYLOAD_LEN_SIZE 1
#define MASK_KEY_SIZE 4

#define DIFF(a, b) (b.ptr - a->ptr)
#define MIN(a, b) (a > b ? b : a)
#define NTOH(n, p) ((n) == 2 ? ntohs(p) : ntohll(p))

data_frame_t *data_frame_new(Arena *a) {
  data_frame_t *frame = arena_alloc(a, sizeof(data_frame_t));
  if (frame == nullptr)
    return nullptr;
  memset(frame, 0, sizeof(data_frame_t));

  frame->state = READING_HEADER;
  frame->header = arena_alloc(a, sizeof(data_frame_header_t));
  memset(frame->header, 0, sizeof(data_frame_header_t));

  return frame;
}

size_t parse_data_frame_header(data_frame_t *frame, str *buf) {
  str msg = STR_WITH_LEN(buf->ptr, buf->len);
  assert(frame->state < READING_PAYLOAD);

  data_frame_header_t *h = frame->header;
  frame_state_t prev_state = frame->state;

  do {
    switch (frame->state) {
    case READING_HEADER:
      if (msg.len < MIN_HEADER_SIZE)
        break;
      h->fin = (msg.ptr[0] >> 7) & 0x01;
      h->opcode = msg.ptr[0] & 0x0F;
      msg = STR_SLICE(&msg, MIN_HEADER_SIZE);
      frame->state = READING_PAYLOAD_SIZE;
      break;

    case READING_PAYLOAD_SIZE:
      if (msg.len < MIN_PAYLOAD_LEN_SIZE)
        break;

      h->mask = (msg.ptr[0] >> 7) & 0x01;
      h->payload_len = msg.ptr[0] & 0x7F;
      msg = STR_SLICE(&msg, MIN_PAYLOAD_LEN_SIZE);

      if (h->payload_len > 125) {
        frame->state = READING_EXTENDED_PAYLOAD_SIZE;
        break;
      }

      frame->state = h->mask ? READING_MASK : READING_PAYLOAD;
      // frame->payload = arena_alloc(arena, h->payload_len);
      // memset(frame->payload, 0, h->payload_len);
      break;

    case READING_EXTENDED_PAYLOAD_SIZE:
      u64 bytes_to_read = h->payload_len == 126 ? 2 : 8;
      if (msg.len < bytes_to_read)
        break;

      memcpy(&h->extended_payload_len, msg.ptr, bytes_to_read);
      msg = STR_SLICE(&msg, bytes_to_read);

      h->extended_payload_len = NTOH(bytes_to_read, h->extended_payload_len);
      // frame->payload = arena_alloc(arena, h->extended_payload_len);
      // memset(frame->payload, 0, h->extended_payload_len);

      frame->state = h->mask ? READING_MASK : READING_PAYLOAD;
      break;
    case READING_MASK:
      if (msg.len < MASK_KEY_SIZE)
        break;

      memcpy(h->masking_key, msg.ptr, MASK_KEY_SIZE);
      msg = STR_SLICE(&msg, MASK_KEY_SIZE);
      frame->state = READING_PAYLOAD;
      break;

    case READING_PAYLOAD:
    case DONE:
      return DIFF(buf, msg);
    }
  } while (prev_state != frame->state && (prev_state = frame->state, 1));

  return DIFF(buf, msg);
}

size_t parse_data_frame_payload(data_frame_t *frame, str *buf,
                                str *out_buffer) {
  assert(frame->state == READING_PAYLOAD);
  assert(out_buffer->len >= PAYLOAD_LEN(frame->header));

  str msg = STR_WITH_LEN(buf->ptr, buf->len);

  data_frame_header_t *h = frame->header;
  frame_state_t prev_state = frame->state;

  do {
    switch (frame->state) {
    case READING_PAYLOAD:
      u64 n = PAYLOAD_LEN(h) - frame->bytes_read;
      if (n == 0) {
        frame->state = DONE;
        break;
      }

      u64 take = MIN(n, msg.len);
      u8 *dest = (u8 *)out_buffer->ptr + frame->bytes_read;
      u8 *src = (u8 *)msg.ptr;

      for (u64 i = 0; i < take; i++) {
        dest[i] = src[i] ^ h->masking_key[(frame->bytes_read + i) % 4];
      }

      frame->bytes_read += take;

      msg = STR_SLICE(&msg, take);
      if (frame->bytes_read == PAYLOAD_LEN(h)) {
        frame->state = DONE;
      }
      break;

    case READING_HEADER:
    case READING_MASK:
    case READING_PAYLOAD_SIZE:
    case READING_EXTENDED_PAYLOAD_SIZE:
    case DONE:
      return DIFF(buf, msg);
    }
  } while (prev_state != frame->state && (prev_state = frame->state, 1));

  return DIFF(buf, msg);
}
