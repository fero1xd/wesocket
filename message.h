#pragma once

#include "data_frame.h"
#include <stddef.h>

#define IS_PENDING_CONTROL(message)                                            \
  (!message.done && message.pending_control_frame)

#define GET_OPCODE(message)                                                    \
  (IS_PENDING_CONTROL(message) ? message.current_frame->header->opcode         \
                               : message.opcode)

#define GET_PAYLOAD_LEN(message)                                               \
  (IS_PENDING_CONTROL(message) ? message.current_frame->header->payload_len    \
                               : message.bytes_read)

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

// - parses data frame one by 1
// - .done property is true if its parsed all fragments of a message
// - .pending_control_frame is true when there is a control frame available to
// - read the caller should always check for control frames as they will be
// overriten by upcomming control frames
size_t parse_message(Arena *a, message_t *message, str *buf);
