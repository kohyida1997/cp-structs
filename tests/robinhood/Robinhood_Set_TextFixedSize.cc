#include <FixedSizeRobinhoodSet.hpp>
#include <IntMurMurHash3.hpp>
#include <TestUtil.hpp>
#include <cstdint>
#include <cstdio>
#include <ios>
#include <iostream>
#include <random>
#include <unordered_set>

template <typename Key, size_t N = 0>
using VectorMap = ykoh::robinhood::robinhood_set_fixed<Key, 0>;

template <typename Key, size_t N>
using ArrayMap = ykoh::robinhood::robinhood_set_fixed<Key, N>;

using namespace ykoh::test_utils;

static std::mt19937 gen32(0);

template <class Entry>
void printEntryAtBucket(const Entry& e, size_t bucketPos) {
  std::cout << "Entry (key = " << e.key << ", bucket = " << bucketPos
            << ", psl = " << e.psl
            << ", occupied = " << (e.isOccupied() ? "YES" : "EMPTY") << ")\n";
}

template <template <class, size_t> class MapTemplate, class KeyT, size_t N>
void testMap() {
  using MapType = MapTemplate<KeyT, N>;
  // Instantiate the map
  MapType testMap = [&]() {
    if constexpr (std::is_same_v<MapType, VectorMap<KeyT>>) {
      return VectorMap<KeyT>{N};
    } else
      return ArrayMap<KeyT, N>{};
  }();

  // Check that the underlying buffer for the buckets are correct
  // Check that IntMurMurHash3 is used for integral keys
  using BufferT = std::remove_reference_t<decltype(testMap.data())>;
  using EntryT = typename decltype(testMap)::EntryT;
  if constexpr (std::is_same_v<MapType, VectorMap<KeyT>>)
    static_assert(std::is_same_v<BufferT, std::vector<EntryT>>);
  else
    static_assert(std::is_same_v<BufferT, std::array<EntryT, N>>);
  if constexpr (std::is_integral_v<KeyT>)
    static_assert(
        std::is_same_v<decltype(testMap.hash_function()), IntMurMurHash3>);

  // Don't fill to the brim yet
  constexpr size_t testMapCapacity = N;
  constexpr size_t sNumKeys = testMapCapacity / 2;
  std::unordered_set<KeyT> s;
  while (s.size() != sNumKeys)
    s.insert(gen32());
  for (auto key : s)
    assertEquals(testMap.insert(key), true);
  for (auto key : s)
    assertEquals(testMap.find(key) != testMap.end(), true);

  // Now fill to the brim
  while (s.size() != testMapCapacity) {
    auto newKey = gen32();
    if (!s.insert(newKey).second)
      continue;
    assertEquals(testMap.insert(newKey), true);
  }

  // All keys match
  for (auto key : s)
    assertEquals(true, testMap.find(key) != testMap.end());

  assertEquals(testMap.size(), testMapCapacity);
  assertEquals(testMap.size(), s.size());
  assertEquals(testMap.isFull(), true);

  // Now delete some and check
  std::unordered_set<KeyT> deleted;
  while (deleted.size() != sNumKeys) {
    auto k = *s.begin();
    deleted.insert(k);
    assertEquals(true, s.find(k) != s.end());
    if (testMap.find(k) == testMap.end()) {
      std::cout << ">>> error could not find key = " << k << std::endl;
    }
    s.erase(k);
    assertEquals(true, testMap.erase(k));
    auto it = testMap.find(k);
    if (it != testMap.end()) {
      std::cout << "!! Just Deleted (Key = " << k << ")\n"
                << "   but found ";
      printEntryAtBucket(*it, std::distance(testMap.data().begin(), it));
    }
  }

  // Ensure deleted keys aren't inside
  for (auto& k : deleted) {
    auto it = testMap.find(k);
    if (it != testMap.end()) {
      std::cout << "!! Already Deleted (Key = " << k << ")\n"
                << "   but found ";
      printEntryAtBucket(*it, std::distance(testMap.data().begin(), it));
    }
    assertEquals(true, testMap.find(k) == testMap.end());
  }

  // Check existing keys still inside
  for (auto& key : s) {
    assertEquals(key, testMap.find(key)->key);
  }

  // Check reset
  testMap.clear();
  for (const auto& e : testMap.data()) {
    assertEquals(0u, e.psl);
    assertEquals(true, e.isEmpty());
  }
  assertEquals(testMap.size(), 0ul);

  std::cout << "Test Passed!" << std::endl;
}

int main() {
  testMap<VectorMap, uint32_t, 10384>();
  testMap<ArrayMap, uint32_t, 10384>();
  testMap<VectorMap, int32_t, 10384>();
  testMap<ArrayMap, int32_t, 10384>();
  testMap<VectorMap, uint64_t, 5192>();
  testMap<ArrayMap, uint64_t, 5192>();
  testMap<VectorMap, int64_t, 5192>();
  testMap<ArrayMap, int64_t, 5192>();
  return 2;
}