#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace hw5 {

struct MarketData {
  std::string symbol{};
  double last_price = -1.0;
  std::uint64_t volume = -1;

  bool empty() const noexcept {
    return symbol.empty() && last_price == -1.0 && volume == -1;
  }
};

class RobinHoodHashTable {
public:
  explicit RobinHoodHashTable(std::size_t initial_capacity = 1024);

  void insert(std::string key, MarketData value);
  bool erase(const std::string &key);
  MarketData *find(const std::string &key);
  const MarketData *find(const std::string &key) const;
  bool contains(const std::string &key) const;

  void clear();
  std::size_t size() const noexcept;
  bool empty() const noexcept;

private:
  struct Entry {
    MarketData data{};
    std::size_t psl = 0;
  };

  std::vector<Entry> table_;
  std::size_t size_;
  std::size_t capacity_;

  std::size_t hash_key(std::string_view key) const;
  void rehash(std::size_t new_capacity);
};

} // namespace hw5
