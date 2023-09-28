#include <ios>
#include <iostream>

#include <FixedSizeRobinhoodSet.hpp>

template <typename Key>
using VectorMap = ykoh::robinhood::robinhood_set_fixed<Key>;

template <typename Key, size_t N>
using ArrayMap = ykoh::robinhood::robinhood_set_fixed<Key, N>;

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

  return 2;
}