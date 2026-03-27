#pragma once

#include "slice.h"

CREATE_NAMED_SLICE(char, str, ((str){.ptr = "", .len = 0}));

#define STR(s) (SLICE(str, s, strlen(s)))
#define STR_LIT(s) &STR(s)

#define STR_BUF(n) &(str){.ptr = (char[n]){0}, .len = n}

#define SV_Fmt "%.*s"
#define SV_Arg(s) (int)(s).len, (s).ptr

#define STR_FIND(haystack, needle) SLICE_FIND(char, str, (haystack), (needle))

#define STR_SPLIT_MUT(slice_var, delimiter, iter_var)                          \
  SLICE_SPLIT_MUT(str, (slice_var), (delimiter), (iter_var))
