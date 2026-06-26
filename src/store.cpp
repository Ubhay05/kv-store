#include "store.h"
#include "lru.h"
#include <iostream>

// We use a global LRU tracker alongside the store
static LRUCache lru_tracker(1000); // why static ? 

Store::Store(size_t max_size) : max_size_(max_size) {}

bool Store::is_expired(const Entry& entry) { //ok
    if (entry.expires_at == 0) return false;
    return std::time(nullptr) >= entry.expires_at;
}

void Store::set(const std::string& key, const std::string& value, int ttl_seconds) {
    std::lock_guard<std::mutex> lock(mutex_); // lock guard locks mutex and releases when out of scope

    evict_if_full();

    Entry entry;
    entry.value      = value;
    entry.expires_at = (ttl_seconds > 0) ? (std::time(nullptr) + ttl_seconds) : 0;

    data_[key] = entry;
    lru_tracker.touch(key);   // mark as recently used
}

std::optional<std::string> Store::get(const std::string& key) { // if found move front in lru , expired remove from both lru,data
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = data_.find(key);
    if (it == data_.end()) return std::nullopt;

    // Lazy expiry check — delete on access if expired
    if (is_expired(it->second)) {
        data_.erase(it);
        lru_tracker.remove(key);
        return std::nullopt;
    }

    lru_tracker.touch(key);   // mark as recently used
    return it->second.value;
}

bool Store::del(const std::string& key) { // del from data_,lru
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = data_.find(key);
    if (it == data_.end()) return false;

    data_.erase(it);
    lru_tracker.remove(key);
    return true;
}

bool Store::exists(const std::string& key) { // checks for key, if exist check expirity
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = data_.find(key);
    if (it == data_.end()) return false;
    if (is_expired(it->second)) {
        data_.erase(it);
        return false;
    }
    return true;
}

void Store::flush() { // intialses new obj to clear lru and clears data_
    std::lock_guard<std::mutex> lock(mutex_);
    data_.clear();
    lru_tracker = LRUCache(max_size_);
}

size_t Store::size() { // gives size
    std::lock_guard<std::mutex> lock(mutex_);
    return data_.size();
}

std::vector<std::string> Store::keys() { // gives all unexpired keys
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> result;
    for (auto& [k, v] : data_) {
        if (!is_expired(v)) result.push_back(k);
    }
    return result;
}

void Store::evict_if_full() { // if full gets the last key from lru and removes it from data_
    // Called inside locked context already
    if (data_.size() < max_size_) return;

    // Ask LRU tracker which key to evict
    std::string lru_key = lru_tracker.evict();
    if (!lru_key.empty()) {
        data_.erase(lru_key);
        std::cout << "[evict] removed LRU key: " << lru_key << "\n";
    }
}