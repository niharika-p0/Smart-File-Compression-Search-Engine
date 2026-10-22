#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <unordered_map>
#include <list>
#include <utility>
#include <iostream>
#include <cstddef>

template <typename Key, typename Value>
class LRUCache
{
private:

    struct CacheEntry
    {
        Key key;
        Value value;
    };

    std::size_t capacity;

    std::list<CacheEntry> cacheList;

    std::unordered_map<
        Key,
        typename std::list<CacheEntry>::iterator
    > cacheMap;

public:

    explicit LRUCache(
        std::size_t capacity = 5
    )
        : capacity(capacity)
    {
    }

    bool get(
        const Key& key,
        Value& value
    )
    {
        auto it = cacheMap.find(key);

        if (it == cacheMap.end())
        {
            return false;
        }

        cacheList.splice(
            cacheList.begin(),
            cacheList,
            it->second
        );

        value = it->second->value;

        return true;
    }

    void put(
        const Key& key,
        const Value& value
    )
    {
        if (capacity == 0)
        {
            return;
        }

        auto it = cacheMap.find(key);

        if (it != cacheMap.end())
        {
            it->second->value = value;

            cacheList.splice(
                cacheList.begin(),
                cacheList,
                it->second
            );

            return;
        }

        if (cacheList.size() >= capacity)
        {
            auto last = cacheList.back();

            cacheMap.erase(last.key);

            cacheList.pop_back();
        }

        cacheList.push_front(
            {
                key,
                value
            }
        );

        cacheMap[key] =
            cacheList.begin();
    }

    bool contains(
        const Key& key
    ) const
    {
        return cacheMap.find(key)
            != cacheMap.end();
    }

    void clear()
    {
        cacheList.clear();
        cacheMap.clear();
    }

    std::size_t size() const
    {
        return cacheList.size();
    }

    void display() const
    {
        for (
            const auto& entry :
            cacheList
        )
        {
            std::cout
                << entry.key
                << " ";
        }

        std::cout << "\n";
    }
};

#endif