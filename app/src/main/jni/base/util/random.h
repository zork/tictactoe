////
// random.h
////

#pragma once

#include "base/basic_types.h"

class Random {
 public:
  static void RandBytes(void* output, int size);
  static Random* get();

  Random(uint32_t seed);
  ~Random();

  uint32_t Next();
  double NextDouble();

 private:
  void Initialize(uint32_t seed);
  void GenerateNumbers();

  // 624 int array to store the state of the generator.
  uint32_t mt_[624];
  int index_;
};
