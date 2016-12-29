////
// macros.h
////

#pragma once

#include <stdint.h>

// Prevent the copy and assign constructors from being used.
// This should be used after the constructors and destructor are declared.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete

// Safely calculate the size of an array at compile time
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))
