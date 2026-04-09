#pragma once

#include "consts.h"
#include "data_frame.h"
#include <stddef.h>

#define FIN 0x8000
#define MASK 0x80

typedef struct {
  bool mask;
  size_t payload_len;
} calculate_opts;

size_t frame_calculate_size(calculate_opts opts);

typedef struct {
  bool fin;

  bool is_masked;
  u8 *mask;

  opcode_t opcode;

  str *payload;
} frame_write_opts;

size_t frame_write(frame_write_opts *opts, str *out);
