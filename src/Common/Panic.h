#pragma once

#include <cstdio>

// this should be supported by both GCC and CLANG equally
#if defined __has_builtin
#if __has_builtin(__builtin_trap)
#define PANIC(msg)                                                                                                     \
    do {                                                                                                               \
        std::fprintf(stderr, "PANIC @[%s:%s:%d] %s", __PRETTY_FUNCTION__, __FILE__, __LINE__, msg);                    \
        __builtin_trap();                                                                                              \
    } while (0)
#endif
#endif

#ifndef PANIC
#error PANIC macro not implemented
#endif
