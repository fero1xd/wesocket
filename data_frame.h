#pragma once

#include "allocator/arena.h"
#include "consts.h"
#include "string/string.h"

typedef enum {
  READING_HEADER,
  READING_PAYLOAD_SIZE,
  READING_EXTENDED_PAYLOAD_SIZE,
  READING_MASK,
  READING_PAYLOAD,
  DONE
} frame_state_t;

struct DataFrameHeader {
  bool fin;
  u8 opcode;
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
  void *payload;
};

typedef struct DataFrame data_frame_t;

data_frame_t *data_frame_new(Arena *arena);
size_t parse_data_frame(Arena *arena, data_frame_t *frame, str *buf);
