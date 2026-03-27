#pragma once

#include "../consts.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  u8 *ptr;
  size_t len;
} Slice;

Slice to_cslice(size_t len, u8 ptr[len]);

#define CREATE_NAMED_SLICE(type, name) __CREATE_NAMED_SLICE(type, name)

#define __CREATE_NAMED_SLICE(type, name)                                       \
  typedef struct {                                                             \
    type *ptr;                                                                 \
    size_t len;                                                                \
  } name;                                                                      \
  name name##_new_len(type *ptr, size_t len) {                                 \
    return (name){.ptr = ptr, .len = len};                                     \
  }                                                                            \
  name name##_new(type *ptr) {                                                 \
    return (name){.ptr = ptr, .len = sizeof(*ptr)};                            \
  }                                                                            \
  /* checks if element exists in the strip set */                              \
  int name##_inStripSet(type c, name strip[static 1]) {                        \
    for (size_t i = 0; i < strip->len; i++) {                                  \
      if (strip->ptr[i] == c)                                                  \
        return 1;                                                              \
    }                                                                          \
    return 0;                                                                  \
  }                                                                            \
                                                                               \
  name name##_trimLeft(name slice[static 1], name strip[static 1]) {           \
    name n = {.ptr = slice->ptr, .len = slice->len};                           \
    while (n.len > 0 && name##_inStripSet(n.ptr[0], strip)) {                  \
      n.ptr++;                                                                 \
      n.len--;                                                                 \
    }                                                                          \
    return n;                                                                  \
  }                                                                            \
                                                                               \
  name name##_trimRight(name slice[static 1], name strip[static 1]) {          \
    name n = {.ptr = slice->ptr, .len = slice->len};                           \
    while (n.len > 0 && name##_inStripSet(n.ptr[n.len - 1], strip)) {          \
      n.len--;                                                                 \
    }                                                                          \
    return n;                                                                  \
  }                                                                            \
                                                                               \
  name name##_trim(name slice[static 1], name strip[static 1]) {               \
    name n = name##_trimLeft(slice, strip);                                    \
    return name##_trimRight(&n, strip);                                        \
  }                                                                            \
                                                                               \
  bool name##_equals(name a[static 1], name b[static 1]) {                     \
    if (a->len != b->len)                                                      \
      return false;                                                            \
    for (size_t i = 0; i < a->len; i++) {                                      \
      if (a->ptr[i] != b->ptr[i])                                              \
        return false;                                                          \
    }                                                                          \
    return true;                                                               \
  }                                                                            \
                                                                               \
  ptrdiff_t name##_findElem(name haystack[static 1], type needle) {            \
    for (size_t i = 0; i < haystack->len; i++) {                               \
      if (haystack->ptr[i] == needle)                                          \
        return (ptrdiff_t)i;                                                   \
    }                                                                          \
    return -1;                                                                 \
  }                                                                            \
                                                                               \
  ptrdiff_t name##_findSlice(name haystack[static 1], name needle[static 1]) { \
    if (needle->len == 0)                                                      \
      return 0;                                                                \
    if (needle->len > haystack->len)                                           \
      return -1;                                                               \
    size_t limit = haystack->len - needle->len;                                \
    for (size_t i = 0; i <= limit; i++) {                                      \
      size_t j = 0;                                                            \
      while (j < needle->len && haystack->ptr[i + j] == needle->ptr[j])        \
        j++;                                                                   \
      if (j == needle->len)                                                    \
        return (ptrdiff_t)i;                                                   \
    }                                                                          \
    return -1;                                                                 \
  }

#define SLICE(name, p, l)                                                      \
  (name) { .ptr = (p), .len = (l) }

#define SLICE_LIT(name, p, l) &SLICE(name, p, l)

#define SLICE_FIND(type, name, haystack, needle)                               \
  _Generic((needle), type: name##_findElem, name *: name##_findSlice)(haystack, needle)

