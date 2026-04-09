#pragma once

#include "allocator/arena.h"
#include "consts.h"
#include "string/string.h"
#include <stdint.h>

#define FIN_MASK 0x80
#define OP_MASK 0x0F
#define MASKING_MASK 0x80
#define PAYLOAD_LEN_MASK 0x7F

typedef enum {
  READING_HEADER,
  READING_PAYLOAD_SIZE,
  READING_EXTENDED_PAYLOAD_SIZE,
  READING_MASK,
  READING_PAYLOAD,
  DONE
} frame_state_t;

typedef enum {
  TEXT = 0x1,
  BINARY = 0x2,
  CONTINUATION = 0x0,
  PING = 0x9,
  PONG = 0xA
} opcode_t;

struct DataFrameHeader {
  bool fin;
  opcode_t opcode;
  bool mask;

  u8 payload_len;
  u64 extended_payload_len;

  u8 masking_key[4];
};

typedef struct DataFrameHeader data_frame_header_t;

struct DataFrame {
  frame_state_t state;
  data_frame_header_t *header;

  u64 bytes_read;
};

typedef struct DataFrame data_frame_t;

data_frame_t *data_frame_new(Arena *arena);
// NOTE: frame->payload_start is only valid till the liftime of the passed in
// buf.ptr, to use it further than that copy the contents to another buffer.
size_t parse_data_frame_header(data_frame_t *frame, str *buf);
size_t parse_data_frame_payload(data_frame_t *frame, str *buf, str *out_buffer);

#define PAYLOAD_LEN(f)                                                         \
  (f->payload_len <= 125 ? f->payload_len : f->extended_payload_len)

#define IS_CONTROL_FRAME(opcode) ((opcode) >= 0x8)
