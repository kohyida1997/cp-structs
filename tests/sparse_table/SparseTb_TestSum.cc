#include "../sparse-table/include/ModuleInfo.hpp"
#include "../sparse-table/include/SparseTableStatic.hpp"
#include "TestUtil.hpp"

#include <bit>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

using u32 = uint32_t;

int main() {
  ykoh::sparse_tb::ModuleInfo().show_desc();

  constexpr auto sumLambda = [](int x, int y) { return x + y; };
  constexpr u32 dataSize = 1024;

  // Setup test data
  auto testData1 = std::vector<int>(dataSize);
  for (uint32_t i = 0; i < testData1.size(); ++i)
    testData1[i] = i;

  // Setup the SparseTable
  auto table1 =
      ykoh::sparse_tb::SparseTableStatic<int, std::bit_width(dataSize) + 1,
                                         dataSize, decltype(sumLambda)>{};
  table1.initTable(testData1);

  // Check all possible ranges for the sums
  for (u32 start = 0; start < testData1.size(); ++start) {
    // End is exclusive
    for (u32 end = start; end <= testData1.size(); ++end) {
      // Gold - use accumulate in O(n)
      auto expected = std::accumulate(testData1.begin() + start,
                                      testData1.begin() + end, 0);

      // Use our sparse table to do it in O(log(n))
      auto actual = table1.computeForRange(start, end, 0);
      ykoh::test_utils::assertEquals(expected, actual);
    }
  }

  std::cout << "Test Passed!" << std::endl;
  return 0;
}