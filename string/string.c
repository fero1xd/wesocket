#include "string.h"
#include <stdio.h>
#include <string.h>

int main(void) {
  str base = STR("  hello world\r\n");
  str trimmed = str_trim(&base, STR_LIT("\r\n "));

  int index = STR_FIND(&trimmed, STR_LIT("worldda"));

  printf(SV_Fmt "\n", SV_Arg(trimmed));
  printf("%ld\n", trimmed.len);
  printf("%ld\n", base.len);
  printf("Found Index: %d\n", index);
}
