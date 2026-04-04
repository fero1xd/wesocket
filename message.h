#pragma once

#include "data_frame.h"
#include <stddef.h>

struct Message {
  // Only for opcodes 0x1(text) and 0x2(binary)
  opcode_t opcode;

  str payload;
  str control_payload;
  data_frame_t *current_frame;

  size_t frames;
  size_t bytes_read;

  bool done;
  bool pending_control_frame;
};

typedef struct Message message_t;

message_t new_message();
size_t parse_message(Arena *a, message_t *message, str *buf);
