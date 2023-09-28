#pragma once

#include <KeyTraits.hpp>
#include <functional>
#include <type_traits>
#include <vector>

namespace ykoh {
namespace robinhood {
// Allocation strategy is determined based on size
// If size (defaults to zero) is 0, then default to dynamic allocation
// using a std::vector, else use a std::array
static constexpr size_t DYNAMIC_SIZE = 0;
template <size_t N>
struct is_dynamic_alloc_size : std::false_type {};
template <>
struct is_dynamic_alloc_size<0> : std::true_type {};

template <class Key, size_t N = DYNAMIC_SIZE, class Traits = KeyTraits<Key>>
requires(N >= DYNAMIC_SIZE) class robinhood_set_fixed {
  // Typedefs
  using HasherFunc = typename Traits::Hasher;
  using KeyEqualCmpFunc = typename Traits::EqualTo;
  using ProbeSeqLenT = uint32_t;
  using OccupiedFlag = bool;
  using PositionT = size_t;
  using SizeT = size_t;
  // Private entry struct
  struct Entry {
    Key key;
    OccupiedFlag occupied : 1;
    ProbeSeqLenT psl : 31;

    inline constexpr bool isEmpty() const noexcept { return !occupied; }
    inline constexpr void setOccupied() noexcept { occupied = true; }
    inline constexpr void setEmpty() noexcept { occupied = false; }
    constexpr Entry() = default;
    constexpr Entry(Key k) : key(std::move(k)), occupied{true}, psl{0} {}
  };
  static_assert(sizeof(Entry) == 8, "oops too big");

  using BucketsT = std::conditional_t<N == DYNAMIC_SIZE,
                                      std::vector<Entry>,
                                      std::array<Entry, N>>;
  using BucketIterT = typename BucketsT::iterator;

  // Members
  SizeT sz{0};
  BucketsT buckets;

  // Private methods
  inline constexpr PositionT computeHomePosition(const Key& k) const {
    return HasherFunc{}(k) % capacity();
  }
  inline constexpr void advancePosition(PositionT& pos) const {
    if (++pos >= capacity())
      pos -= capacity();
  }

 public:
  // Default construct enabled only if using static alloc
  // Note that for SFINAE to work, it has to check based on template params
  // of the function itself, not at the class level. Hence we use _N = N
  template <
      size_t _N = N,
      class = typename std::enable_if_t<!is_dynamic_alloc_size<_N>::value>>
  constexpr robinhood_set_fixed() : buckets{} {}

  // Param construct enabled only if using dynamic alloc (uses std::vector)
  template <size_t _N = N,
            class = typename std::enable_if_t<is_dynamic_alloc_size<_N>::value>>
  constexpr explicit robinhood_set_fixed(SizeT fixedCapacity)
      : buckets(fixedCapacity) {}

  // Copies key, returns true if inserted
  inline bool insert(Key k) {
    // full, cannot insert anymore
    if (sz == capacity())
      return false;

    Entry incomingEntry(k);
    auto insertPos = computeHomePosition(k);
    // loop until find an empty spot
    while (!buckets[insertPos].isEmpty()) {
      auto& existingEntry = buckets[insertPos];
      // already inserted before, return false
      if (KeyEqualCmpFunc{}(existingEntry.key, incomingEntry.key))
        return false;

      // If existingEntry.psl >= incomingEntry.psl, it means the
      // existingEntry's homePos is before/equal to the incomingEntry's
      // homePos, and thus doesn't require any displacement

      // displace existing element, insert the incoming at this position
      if (existingEntry.psl < incomingEntry.psl)
        std::swap(existingEntry, incomingEntry);

      ++incomingEntry.psl;
      advancePosition(insertPos);
    }
    buckets[insertPos] = incomingEntry;
    return ++sz, true;
  }

  // inline bool erase(Key k) noexcept;
  inline auto find(Key k) noexcept {
    auto searchPos = computeHomePosition(k);
    ProbeSeqLenT currPsl = 0;
    while (currPsl < capacity() && !buckets[searchPos].isEmpty() &&
           buckets[searchPos].psl >= currPsl) {
      auto& currEntry = buckets[searchPos];
      if (KeyEqualCmpFunc{}(currEntry.key, k))
        return begin() + searchPos;
      ++currPsl, advancePosition(searchPos);
    }

    return end();
  }

  // Capacity
  inline constexpr size_t capacity() const noexcept { return buckets.size(); }

  // Iters
  auto begin() noexcept { return buckets.begin(); }
  auto end() noexcept { return buckets.end(); }

  void clear() noexcept {
    for (auto& e : buckets) {
      e.setEmpty();
      e.psl = 0;
    }
  }
};
}  // namespace robinhood
}  // namespace ykoh