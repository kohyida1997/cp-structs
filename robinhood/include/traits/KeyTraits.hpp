#pragma once

#include <IntMurMurHash3.hpp>
#include <type_traits>

template <class KeyType>
struct KeyTraits {
  // use IntMurMurHash3 if is integral
  using Hasher = std::conditional_t<std::is_integral<KeyType>::value,
                                    IntMurMurHash3,
                                    std::hash<KeyType>>;
  using EqualTo = std::equal_to<KeyType>;
};
