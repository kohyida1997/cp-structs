#include "../sparse-table/include/ModuleInfo.hpp"
#include "../sparse-table/include/SparseTableStatic.hpp"
#include "TestUtil.hpp"

#include <bit>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <vector>

using u32 = uint32_t;

template <typename LambdaFunc, typename T> void testWithFunc(T funcInit) {

  constexpr u32 dataSize = 1024;

  std::mt19937 gen32(std::random_device{}());

  // Setup test data
  auto testData1 = std::vector<u32>(dataSize);
  for (uint32_t i = 0; i < dataSize; ++i)
    testData1[i] = gen32() % std::numeric_limits<u32>::max();

  // Setup the SparseTable
  auto table1 =
      ykoh::sparse_tb::SparseTableStatic<u32, std::bit_width(dataSize) + 1,
                                         dataSize, LambdaFunc>{};
  table1.initTable(testData1);

  auto funcInstance = LambdaFunc{};

  // Check all possible ranges for the sums
  for (u32 start = 0; start < testData1.size(); ++start) {
    // end is exclusive
    for (u32 end = start + 1; end <= testData1.size(); ++end) {
      auto expected =
          std::accumulate(testData1.begin() + start, testData1.begin() + end,
                          funcInit, funcInstance);
      auto actual = table1.computeOverlappingForRange(start, end, funcInit);
      ykoh::test_utils::assertEquals(expected, actual);
    }
  }

  std::cout << "Test Passed!" << std::endl;
}

int main() {
  ykoh::sparse_tb::ModuleInfo().show_desc();
  auto minWrap = [](auto x, auto y) { return std::min(x, y); };
  testWithFunc<decltype(minWrap)>(std::numeric_limits<u32>::max());
  auto maxWrap = [](auto x, auto y) { return std::max(x, y); };
  testWithFunc<decltype(maxWrap)>(std::numeric_limits<u32>::min());
  return 0;
}