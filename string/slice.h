#pragma once

#include "../consts.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  u8 *ptr;
  size_t len;
} Slice;

#define CREATE_NAMED_SLICE(type, name, ini)                                    \
  __CREATE_NAMED_SLICE(type, name, ini)

#define __CREATE_NAMED_SLICE(type, name, ini)                                  \
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
  bool name##_is_null(name *slice) {                                           \
    return slice->ptr == NULL && slice->len <= 0;                              \
  }                                                                            \
  name name##_slice(name slice[static 1], size_t start, size_t end) {          \
    if (end > slice->len)                                                      \
      end = slice->len;                                                        \
    if (start > end)                                                           \
      return ini;                                                              \
                                                                               \
    return name##_new_len(slice->ptr + start, end - start);                    \
  }                                                                            \
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
  }                                                                            \
  name name##_split_mut(name slice[static 1], name seq[static 1]) {            \
    ptrdiff_t index = name##_findSlice(slice, seq);                            \
    if (index < 0)                                                             \
      return name##_new_len(NULL, 0);                                          \
    name result = name##_slice(slice, index + 1, slice->len);                  \
    slice->len = (size_t)index;                                                \
    return result;                                                             \
  }

#define SLICE(name, p, l)                                                      \
  (name) { .ptr = (p), .len = (l) }

#define SLICE_LIT(name, p, l) &SLICE(name, p, l)

#define SLICE_FIND(type, name, haystack, needle)                               \
  _Generic((needle), type: name##_findElem, name *: name##_findSlice)(         \
      haystack, needle)

#define SLICE_SPLIT_MUT(name, slice, split_by, iter)                           \
  for (name src = *(slice), other = name##_split_mut(&src, split_by);          \
       src.ptr != NULL && (*(iter) = src, 1);                                  \
       src = other, other = name##_split_mut(&src, split_by))
