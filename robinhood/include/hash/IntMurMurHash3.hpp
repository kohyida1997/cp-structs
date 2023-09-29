#pragma once

#include <cstdint>
#include <cstddef>

// Stolen from:
// https://github.com/martinus/robin-hood-hashing/blob/master/src/include/robin_hood.h
struct IntMurMurHash3 {
  inline size_t operator()(uint64_t x) noexcept {
    // tried lots of different hashes, let's stick with murmurhash3. It's
    // simple, fast, well tested, and doesn't need any special 128bit
    // operations.
    x ^= x >> 33U;
    x *= UINT64_C(0xff51afd7ed558ccd);
    x ^= x >> 33U;
    x *= UINT64_C(0xc4ceb9fe1a85ec53);
    x ^= x >> 33U;
    return static_cast<size_t>(x);
  }
};
