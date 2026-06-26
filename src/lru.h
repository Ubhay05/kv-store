#pragma once
#include <string>
#include <unordered_map>
#include <list>

// LRU Cache with hashmap + doubly linked list
class LRUCache {
public:
    LRUCache(size_t capacity);

    void        touch(const std::string& key);   // mark key as recently used
    void        remove(const std::string& key);  // remove key from tracker
    std::string evict();                         // remove and return LRU key
    size_t      size() const;

private:
    size_t                                               capacity_;
    std::list<std::string>                               order_;   // front = MRU, back = LRU
    std::unordered_map<std::string,
        std::list<std::string>::iterator>                map_;     // key → iterator in list
};