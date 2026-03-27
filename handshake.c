#include "handshake.h"
#include "string/base64.h"
#include "string/string.h"
#include <assert.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "string/base64.c"

#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define MAGIC_KEY_LENGTH 36
#define WS_KEY_LENGTH 24

void make_handshake_accept_key(str *ws_key, str *out) {
  assert(out->len >= 20);

  str *combine = STR_BUF(WS_KEY_LENGTH + MAGIC_KEY_LENGTH);
  memcpy(combine->ptr, ws_key->ptr, WS_KEY_LENGTH);
  memcpy(combine->ptr + WS_KEY_LENGTH, MAGIC_KEY, MAGIC_KEY_LENGTH);

  SHA1((unsigned char *)combine->ptr, combine->len, (unsigned char *)out->ptr);
}

int main(void) {
  str *hash = STR_BUF(20);
  make_handshake_accept_key(STR_LIT("dGhlIHNhbXBsZSBub25jZQ=="), hash);

  size_t output_len = 0;
  char *encoded =
      base64_encode((unsigned char *)hash->ptr, hash->len, &output_len);

  printf(SV_Fmt "\n", SV_Arg(*hash));
  printf("L: %ld\n", hash->len);

  printf("----\n");
  printf("%s\n", encoded);
  printf("%ld\n", output_len);

  return 0;
}
