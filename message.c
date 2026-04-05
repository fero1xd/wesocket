#include "message.h"
#include "data_frame.h"
#include "string/string.h"
#include <assert.h>
#include <string.h>

#define MIN_PAYLOAD_LEN 125

message_t new_message() {
  return (message_t){
      .frames = 0,
      .done = false,
      .opcode = 0,
      .payload = STR_NULL(),
      .current_frame = NULL,
      .bytes_read = 0,
      .control_payload = STR_NULL(),
      .pending_control_frame = false,
  };
}

size_t parse_message(Arena *a, message_t *message, str *buf) {
  assert(message != NULL);
  assert(!message->done);

  if (message->current_frame == NULL || message->current_frame->state == DONE) {
    message->current_frame = data_frame_new(a);
    message->pending_control_frame = false;
  }

  data_frame_t *frame = message->current_frame;
  frame_state_t prev_state = frame->state;
  size_t read = 0;

  do {
    str slice = STR_SLICE(buf, read);

    if (frame->state < READING_PAYLOAD) {
      read += parse_data_frame_header(frame, &slice);
      if (frame->state == READING_PAYLOAD) {

        if (IS_CONTROL_FRAME(frame->header->opcode)) {
          assert(frame->header->fin);
          assert(PAYLOAD_LEN(frame->header) <= 125);

          message->pending_control_frame = true;
          message->control_payload =
              STR_WITH_LEN(arena_alloc(a, PAYLOAD_LEN(frame->header)),
                           PAYLOAD_LEN(frame->header));
          continue;
        }

        if (message->frames == 0) {
          message->opcode = frame->header->opcode;
        } else {
          assert(frame->header->opcode == CONTINUATION);

          size_t new_sz = message->payload.len + PAYLOAD_LEN(frame->header);
          message->payload =
              STR_WITH_LEN(arena_realloc(a, message->payload.ptr,
                                         message->payload.len, new_sz),
                           new_sz);
          break;
        }

        size_t payload_size = PAYLOAD_LEN(frame->header);
        message->payload =
            STR_WITH_LEN(arena_alloc(a, payload_size), payload_size);
      }
    } else if (frame->state == READING_PAYLOAD) {
      if (message->pending_control_frame) {
        read +=
            parse_data_frame_payload(frame, &slice, &message->control_payload);

      } else {
        str out = STR_SLICE(&message->payload, message->bytes_read);
        read += parse_data_frame_payload(frame, &slice, &out);

        if (frame->state == DONE) {
          message->frames++;
          message->bytes_read += frame->bytes_read;
          message->done = frame->header->fin;
        }
      }
    }
  } while (prev_state != frame->state && (prev_state = frame->state, 1));

  return read;
}
