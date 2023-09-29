#pragma once

#include <sstream>
#include <stdexcept>
#include <type_traits> 
namespace ykoh {
namespace test_utils {

template <typename T>
concept EqualComparable = requires(T x, T y) {
  x == y;
  x != y;
};

template <EqualComparable T> void assertEquals(T expected, T actual) {
  if (expected != actual) {
    std::stringstream ss;
    ss << "Assertion failed: ";
    ss << "Expected[" << expected << "], ";
    ss << "Actual[" << actual << "]\n";
    throw std::runtime_error(ss.str());
  }
}

} // namespace test_utils
} // namespace ykoh