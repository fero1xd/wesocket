#pragma once

#include "slice.h"

CREATE_NAMED_SLICE(char, str);

#define STR(s) SLICE(str, s, strlen(s))
#define STR_LIT(s) &STR(s)

#define SV_Fmt "%.*s"
#define SV_Arg(s) (int)(s).len, (s).ptr

#define STR_FIND(haystack, needle) SLICE_FIND(char, str, (haystack), (needle))
