#pragma once

#include "../consts.h"
#include <stddef.h>

typedef struct {
  u8 *ptr;
  size_t len;
} Slice;

Slice to_cslice(size_t len, u8 ptr[len]);

#define CREATE_NAMED_SLICE

#ifdef SLICE_IMPL

#endif // SLICE_IMPL
