#include "lru.h"

LRUCache::LRUCache(size_t capacity) : capacity_(capacity) {} // member init list

void LRUCache::touch(const std::string& key) {
    // If key already exists, remove it from current position
    auto it = map_.find(key);
    if (it != map_.end()) {
        order_.erase(it->second);
        map_.erase(it);
    }
    // Insert at front (most recently used)
    order_.push_front(key);
    map_[key] = order_.begin();
}

void LRUCache::remove(const std::string& key) {// to remove from anywhere
    auto it = map_.find(key);
    if (it == map_.end()) return;
    order_.erase(it->second);
    map_.erase(it);
}

std::string LRUCache::evict() { // when full remove the least recently used ie last in order_   // this func called when full
    if (order_.empty()) return "";
    // Back of list = least recently used
    std::string lru_key = order_.back();
    order_.pop_back();
    map_.erase(lru_key);
    return lru_key;
}

size_t LRUCache::size() const { //curr size not capacity
    return map_.size();
}