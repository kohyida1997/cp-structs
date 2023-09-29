#include <FixedSizeRobinhoodSet.hpp>
#include <IntMurMurHash3.hpp>
#include <TestUtil.hpp>
#include <cstdint>
#include <ios>
#include <iostream>
#include <random>
#include <unordered_set>

template <typename Key>
using VectorMap = ykoh::robinhood::robinhood_set_fixed<Key>;

template <typename Key, size_t N>
using ArrayMap = ykoh::robinhood::robinhood_set_fixed<Key, N>;

using namespace ykoh::test_utils;

static std::mt19937 gen32(2);

void testVectorMap() {
  constexpr size_t vMapCapacity = 5192;
  VectorMap<uint32_t> vMap{vMapCapacity};
  static_assert(std::is_same_v<decltype(vMap.hash_function()), IntMurMurHash3>);
  // Don't fill to the brim yet
  constexpr size_t sNumKeys = vMapCapacity / 2;
  std::unordered_set<uint32_t> s;
  while (s.size() != sNumKeys)
    s.insert(gen32());
  for (auto key : s)
    assertEquals(vMap.insert(key), true);
  for (auto key : s)
    assertEquals(vMap.find(key) != vMap.end(), true);

  // Now fill to the brim
  while (s.size() != vMapCapacity) {
    auto newKey = gen32();
    if (!s.insert(newKey).second) continue;
    assertEquals(vMap.insert(newKey), true);
  }

  // All keys match
  for (auto key : s) {
    assertEquals(true, vMap.find(key) != vMap.end());
  }

  assertEquals(vMap.size(), vMapCapacity);
  assertEquals(vMap.size(), s.size());
  assertEquals(vMap.isFull(), true);

  // Now delete some and check
  std::unordered_set<uint32_t> deleted;
  while (deleted.size() != sNumKeys) {
    auto k = *s.begin();
    deleted.insert(k);
    assertEquals(true, s.find(k) != s.end());
    if (vMap.find(k) == vMap.end()) {
      std::cout << ">>> error could not find key = " << k << std::endl;
    }
    s.erase(k);
    assertEquals(true, vMap.erase(k));
  }
  std::cout << "Test Passed!" << std::endl;
}

int main() {
  VectorMap<int> vMap{32};
  ArrayMap<int, 16> aMap{};

  vMap.insert(4);
  std::cout << "vMap contains 4? --> " << std::boolalpha
            << (vMap.find(4) != vMap.end()) << std::endl;
  std::cout << "vMap contains 25? --> " << std::boolalpha
            << (vMap.find(25) != vMap.end()) << std::endl;

  aMap.insert(25);
  std::cout << "aMap contains 4? --> " << std::boolalpha
            << (aMap.find(4) != aMap.end()) << std::endl;
  std::cout << "aMap contains 25? --> " << std::boolalpha
            << (aMap.find(25) != aMap.end()) << std::endl;

  testVectorMap();

  return 2;
}