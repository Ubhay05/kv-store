#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <ctime>

// Value stored alongside optional expiry timestamp
struct Entry {
    std::string value;
    time_t      expires_at;  // 0 = no expiry
};

class Store {
public:
    Store(size_t max_size = 1000);

    // Core operations
    void        set(const std::string& key, const std::string& value, int ttl_seconds = 0);
    std::optional<std::string> get(const std::string& key);
    bool        del(const std::string& key);
    bool        exists(const std::string& key);
    void        flush();                  // delete everything
    size_t      size();

    // Dump all keys (for debugging)
    std::vector<std::string> keys();

private:
    std::unordered_map<std::string, Entry> data_;
    size_t                                  max_size_;
    std::mutex                              mutex_;

    bool   is_expired(const Entry& entry);
    void   evict_if_full();
};