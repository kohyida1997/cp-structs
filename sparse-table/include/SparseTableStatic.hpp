#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <stdexcept>

namespace ykoh {
namespace sparse_tb {

namespace detail {
int fastLog2Floor(size_t num) { return std::bit_width(num) - 1; }
} // namespace detail

// Takes in lambda as templated type
template <typename T, size_t K, size_t MAXN, class Func>
class SparseTableStatic {
  // static asserts
  static_assert(K >= (std::bit_width(MAXN) - 1));

  // using defs
  using data_t = T;
  using lambda_t = Func;
  using table_t = std::array<std::array<data_t, MAXN>, K>;
  table_t tb;

  template <typename Iterable> bool validateSize(const Iterable &data) {
    auto size = std::distance(data.begin(), data.end());
    return size >= 0 && static_cast<size_t>(size) <= MAXN;
  }

public:
  // Default ctr
  explicit SparseTableStatic() noexcept {}

  template <typename Iterable>
  explicit SparseTableStatic(const Iterable &data) {
    initTable(data);
  }

  // Initialize the sparse table in O(N * logN) time where N = data.size()
  template <typename Iterable> void initTable(const Iterable &data) {
    // Need to validate first
    if (!validateSize(data)) {
      throw std::runtime_error(
          "Input data size is too large for this Sparse Table");
    }

    auto sz = static_cast<size_t>(std::distance(data.begin(), data.end()));
    auto maxI = static_cast<size_t>(detail::fastLog2Floor(sz));
    auto funcInstance = lambda_t{};

    /*
        Use DP to initialize the table
        arr[i][j] =
            Func(arr[i-1][j], arr[i-1][j+2^(i-1)])
    */

    // Base case (assumed to be true)
    // arr[0][j] = data[j] for ALL j
    std::copy(data.begin(), data.end(), tb[0].begin());

    // General case
    for (size_t i = 1; i <= maxI; ++i) {
      for (size_t j = 0; j < sz; ++j) {
        tb[i][j] = funcInstance(tb[i - 1][j], tb[i - 1][j + (1 << (i - 1))]);
      }
    }
  }

  // Compute func(left, right) for range  [left, right)
  // Pre-condition: right >= left && left >= 0 && MAXN >= right
  data_t computeForRange(size_t left, size_t right, data_t identity) {
    auto intervalSize = right - left;
    auto largestPow = detail::fastLog2Floor(intervalSize);
    auto x = left;
    auto y = right;
    auto funcInstance = lambda_t{};
    data_t res = identity;

    for (auto i = largestPow; i >= 0; --i) {
      size_t currIntervalSize = 1 << i;
      if (currIntervalSize <= (y - x)) {
        res = funcInstance(res, tb[i][x]);
        x += currIntervalSize;
      }
    }

    return res;
  }
};

} // namespace sparse_tb
} // namespace ykoh
