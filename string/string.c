#include "string.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  str base = STR("  hello world\r\n");
  str trimmed = str_trim(&base, STR_LIT("\r\n "));
  //
  // int index = STR_FIND(&trimmed, STR_LIT("world"));
  // // str sliced = str_slice(&trimmed, index, trimmed.len);
  // str second = str_split_mut(&trimmed, STR_LIT(" "));

  // iter = str_split(&trim, ",")
  // while(l = iter.next()) {...}
  //
  // ini
  str iter;
  STR_SPLIT_MUT(&trimmed, STR_LIT(" "), &iter) {
    printf(SV_Fmt "\n", SV_Arg(iter));
  }
}
