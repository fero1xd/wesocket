#pragma once

#include "allocator/arena.h"
#include "string/string.h"

str make_handshake_accept_key(Arena *arena, str *ws_key);
