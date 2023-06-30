#include "../sparse-table/include/ModuleInfo.hpp"
#include "../sparse-table/include/SparseTableStatic.hpp"

#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

int main() {
  ykoh::sparse_tb::ModuleInfo().show_desc();

  constexpr auto sumLambda = [](int x, int y) { return x + y; };

  // Setup test data
  auto testData1 = std::vector<int>(64);
  for (uint32_t i = 0; i < testData1.size(); ++i)
    testData1[i] = i;

  // Setup the SparseTable
  auto table1 =
      ykoh::sparse_tb::SparseTableStatic<int, 8, 128, decltype(sumLambda)>{};
  table1.initTable(testData1);

  using u32 = uint32_t;

  // Check all possible ranges for the sums
  for (u32 start = 0; start < testData1.size(); ++start) {
    for (u32 end = start; end <= testData1.size(); ++end) {
      auto expected = std::accumulate(testData1.begin() + start,
                                      testData1.begin() + end, 0);
      auto actual = table1.computeForRange(start, end, 0);
      if (actual != expected) {
        std::stringstream ss;
        ss << "Assertion failed: ";
        ss << "Expected[" << expected << "], ";
        ss << "Actual[" << actual << "]\n";
        throw std::runtime_error(ss.str());
      }
    }
  }

  std::cout << "Test Passed!" << std::endl;
  return 0;
}