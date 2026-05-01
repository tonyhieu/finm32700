#include "hw5/robin_hood_hash_table.hpp"

#include <functional>
#include <utility>

hw5::RobinHoodHashTable::RobinHoodHashTable(std::size_t initial_capacity)
    : table_(initial_capacity == 0 ? 8 : initial_capacity), size_(0),
      capacity_(initial_capacity == 0 ? 8 : initial_capacity) {}

std::size_t hw5::RobinHoodHashTable::hash_key(std::string_view key) const {
  if (capacity_ == 0) {
    return 0;
  }
  return std::hash<std::string_view>{}(key) % capacity_;
}

void hw5::RobinHoodHashTable::insert(std::string key, MarketData value) {
  if (capacity_ == 0) {
    rehash(8);
  }
  if ((size_ + 1) * 10 >= capacity_ * 7) {
    rehash(capacity_ * 2);
  }

  Entry incoming;
  incoming.data = std::move(value);
  incoming.data.symbol = key;
  incoming.psl = 0;

  std::size_t index = hash_key(key);
  while (true) {
    Entry &slot = table_[index];
    if (slot.data.empty()) {
      slot = std::move(incoming);
      ++size_;
      return;
    }

    if (slot.data.symbol == key) {
      slot.data = std::move(incoming.data);
      slot.data.symbol = key;
      return;
    }

    if (slot.psl < incoming.psl) {
      std::swap(slot, incoming);
    }

    index = (index + 1) % capacity_;
    ++incoming.psl;
  }
}

bool hw5::RobinHoodHashTable::erase(const std::string &key) {
  if (size_ == 0 || capacity_ == 0) {
    return false;
  }

  std::size_t index = hash_key(key);
  std::size_t dist = 0;
  while (true) {
    Entry &slot = table_[index];
    if (slot.data.empty() || dist > slot.psl) {
      return false;
    }
    if (slot.data.symbol == key) {
      std::size_t current = index;
      std::size_t next = (current + 1) % capacity_;

      while (!table_[next].data.empty() && table_[next].psl > 0) {
        table_[current] = std::move(table_[next]);
        --table_[current].psl;
        current = next;
        next = (next + 1) % capacity_;
      }

      table_[current] = Entry{};
      --size_;
      return true;
    }
    index = (index + 1) % capacity_;
    ++dist;
  }
}

hw5::MarketData *hw5::RobinHoodHashTable::find(const std::string &key) {
  if (size_ == 0 || capacity_ == 0) {
    return nullptr;
  }

  std::size_t index = hash_key(key);
  std::size_t dist = 0;
  while (true) {
    Entry &slot = table_[index];
    if (slot.data.empty() || dist > slot.psl) {
      return nullptr;
    }
    if (slot.data.symbol == key) {
      return &slot.data;
    }
    index = (index + 1) % capacity_;
    ++dist;
  }
}

const hw5::MarketData *
hw5::RobinHoodHashTable::find(const std::string &key) const {
  if (size_ == 0 || capacity_ == 0) {
    return nullptr;
  }

  std::size_t index = hash_key(key);
  std::size_t dist = 0;
  while (true) {
    const Entry &slot = table_[index];
    if (slot.data.empty() || dist > slot.psl) {
      return nullptr;
    }
    if (slot.data.symbol == key) {
      return &slot.data;
    }
    index = (index + 1) % capacity_;
    ++dist;
  }
}

bool hw5::RobinHoodHashTable::contains(const std::string &key) const {
  return find(key) != nullptr;
}

void hw5::RobinHoodHashTable::clear() {
  table_.assign(capacity_, Entry{});
  size_ = 0;
}

std::size_t hw5::RobinHoodHashTable::size() const noexcept { return size_; }

bool hw5::RobinHoodHashTable::empty() const noexcept { return size_ == 0; }

void hw5::RobinHoodHashTable::rehash(std::size_t new_capacity) {
  if (new_capacity < 8) {
    new_capacity = 8;
  }

  std::vector<Entry> old_table = std::move(table_);
  table_.assign(new_capacity, Entry{});
  capacity_ = new_capacity;
  size_ = 0;

  for (auto &entry : old_table) {
    if (!entry.data.empty()) {
      insert(entry.data.symbol, std::move(entry.data));
    }
  }
}
