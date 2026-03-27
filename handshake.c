#include "handshake.h"
#include "string/base64.h"
#include "string/string.h"
#include <assert.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define MAGIC_KEY_LENGTH 36
#define WS_KEY_LENGTH 24

str make_handshake_accept_key(Arena *arena, str *ws_key) {
  str *combine = STR_BUF(WS_KEY_LENGTH + MAGIC_KEY_LENGTH);
  memcpy(combine->ptr, ws_key->ptr, WS_KEY_LENGTH);
  memcpy(combine->ptr + WS_KEY_LENGTH, MAGIC_KEY, MAGIC_KEY_LENGTH);

  str *hash = STR_BUF(20);

  SHA1((unsigned char *)combine->ptr, combine->len, (unsigned char *)hash->ptr);

  size_t output_len = 0;
  char *encoded =
      base64_encode(arena, (unsigned char *)hash->ptr, hash->len, &output_len);

  return *STR_WITH_LEN(encoded, output_len);
}
