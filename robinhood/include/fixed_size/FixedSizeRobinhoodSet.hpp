#pragma once

#include <KeyTraits.hpp>
#include <algorithm>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <vector>

#define DEBUG 0
#if DEBUG
#include <iostream>
#endif

namespace ykoh {
namespace robinhood {
// Allocation strategy is determined based on size
// If size (defaults to zero) is 0, then default to dynamic allocation
// using a std::vector, else use a std::array
static constexpr size_t DYNAMIC_SIZE = 0;
static consteval bool isDynamicAllocSize(size_t N) {
  return N == 0;
}
static consteval bool isStaticAllocSize(size_t N) {
  return N > 0;
}

template <class Key, size_t N = DYNAMIC_SIZE, class Traits = KeyTraits<Key>>
requires(isDynamicAllocSize(N) or
         isStaticAllocSize(N)) class robinhood_set_fixed {
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

    inline constexpr bool isOccupied() const noexcept { return occupied; }
    inline constexpr bool isEmpty() const noexcept { return !isOccupied(); }
    inline constexpr void setOccupied() noexcept { occupied = true; }
    inline constexpr void setEmpty() noexcept { occupied = false; }
    inline constexpr void reset() noexcept {
      setEmpty();
      psl = 0;
      // Note key is not reset
    }
    constexpr Entry() = default;
    constexpr Entry(Key k) : key(std::move(k)), occupied{true}, psl{0} {}
  };

  using BucketsT = std::conditional_t<isDynamicAllocSize(N),
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
  auto begin() noexcept { return buckets.begin(); }

#if DEBUG
  void printEntryAtBucket(const Entry& e, PositionT bucketPos) {
    std::cout << "Entry (key = " << e.key << ", bucket = " << bucketPos
              << ", psl = " << e.psl
              << ", occupied = " << (e.isOccupied() ? "YES" : "EMPTY")
              << ", home = " << computeHomePosition(e.key) << ")\n";
  }
#endif

 public:
  using EntryT = Entry;
  // Default construct enabled only if using static alloc
  // Note that for SFINAE to work, it has to check based on template params
  // of the function itself, not at the class level. Hence we use _N = N
  template <size_t _N = N>
  requires(isStaticAllocSize(_N)) constexpr robinhood_set_fixed() : buckets{} {}

  // Param construct enabled only if using dynamic alloc (uses std::vector)
  template <size_t _N = N>
  requires(isDynamicAllocSize(_N)) constexpr explicit robinhood_set_fixed(
      SizeT fixedCapacity)
      : buckets(fixedCapacity) {}

  // Copies key, returns true if inserted
  inline bool insert(Key k) {
    // full, cannot insert anymore
    if (isFull())
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
    buckets[insertPos] = std::move(incomingEntry);
    return ++sz, true;
  }

  // Find returns an iterator to the Entry
  inline auto find(const Key& k) noexcept {
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

  // Erase returns true if the requested Key is found and deleted
  // Does backshift deletion to avoid tombstones
  inline bool erase(const Key& k) noexcept {
    // Find the key first
    auto it = find(k);
    if (it == end())
      return false;

    PositionT bucketIdxToDelete = std::distance(begin(), it);
#if DEBUG
    std::cout << ">> Erasing ";
    printEntryAtBucket(*it, bucketIdxToDelete);
#endif
    PositionT currPos = bucketIdxToDelete;
    PositionT nextPos = currPos;
    advancePosition(nextPos);
    // Shift backwards until no more key (wraps around)
    for (; nextPos != bucketIdxToDelete && buckets[nextPos].isOccupied() &&
           buckets[nextPos].psl > 0;
         advancePosition(currPos), advancePosition(nextPos)) {
      auto& nextElem = buckets[nextPos];
      auto& currElem = buckets[currPos];
#if DEBUG
      std::cout << ">> Shifting ";
      printEntryAtBucket(nextElem, nextPos);
#endif
      currElem = std::move(nextElem);
      currElem.psl--;
    }
    buckets[currPos].setEmpty();
    return --sz, true;
  }

  // Capacity and fullness
  inline constexpr SizeT capacity() const noexcept {
    return buckets.size();
  }
  inline bool isFull() const noexcept {
    return sz == capacity();
  }
  inline constexpr SizeT size() const noexcept {
    return sz;
  }

  // Iters
  auto end() noexcept {
    return buckets.end();
  }

  void clear() noexcept {
    sz = 0;
    for (auto& e : buckets)
      e.reset();
  }

  // Accessor to raw buffer
  inline constexpr BucketsT& data() {
    return buckets;
  }

  // Observers
  inline constexpr auto hash_function() noexcept {
    return HasherFunc{};
  }
};
}  // namespace robinhood
}  // namespace ykoh
