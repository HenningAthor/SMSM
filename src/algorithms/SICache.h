#ifndef SMSM_SICACHE_H
#define SMSM_SICACHE_H

#include <cstddef>
#include <utility>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <iostream>

/**
 * Represents a cached entry storing a vector and score improvement.
 *
 * @tparam TypeSF The numeric type used for the 'score_improvement' attribute (e.g., int, float, double).
 */
template<typename TypeSF>
struct CacheEntry {
    std::vector<size_t> vec;
    TypeSF score_improvement = 0.0;

    /**
     * Default constructor for CacheEntry.
     */
    inline CacheEntry() = default;

    /**
     * Constructor for CacheEntry that initializes the attributes.
     *
     * @param v The vector.
     * @param si The score improvement.
     */
    inline CacheEntry(std::vector<size_t> &v, TypeSF si) {
        vec = v;
        score_improvement = si;
    }
};

/**
 * Represents a cached entry storing two elements and score improvement.
 *
 * @tparam TypeSF The numeric type used for the 'score_improvement' attribute (e.g., int, float, double).
 */
template<typename TypeSF>
struct CacheEntry2D {
    size_t x1 = 0;
    size_t x2 = 0;
    TypeSF score_improvement = 0.0;

    /**
     * Default constructor for CacheEntry2D.
     */
    inline CacheEntry2D() = default;

    /**
     * Constructor for CacheEntry2D that initializes the attributes.
     *
     * @param x The value of the first element.
     * @param y The value of the second element.
     * @param si The score improvement.
     */
    inline CacheEntry2D(size_t x, size_t y, TypeSF si) {
        x1 = x;
        x2 = y;
        score_improvement = si;
    }
};

/**
 * Cache for storing score improvement entries.
 *
 * @tparam TypeSF The numeric type used for score improvement attributes (e.g., int, float, double).
 */
template<typename TypeSF>
class SICache {
public:
    std::unordered_map<size_t, CacheEntry<TypeSF>> hash_map;
    std::unordered_map<size_t, CacheEntry2D<TypeSF>> hash_map_2D;

    /**
     * Clears the cache, removing all stored entries.
     */
    inline void clear() {
        hash_map.clear();
        hash_map_2D.clear();
    }

    /**
     * Checks if a entry with the specified vector exists in the cache.
     *
     * @param v A vector of indices or values to check for in the cache.
     * @return True if the entry exists and matches the provided vector; false otherwise.
     */
    inline TypeSF get_entry(size_t hash, std::vector<size_t> &v) {
        auto iter = hash_map.find(hash);

        if (iter != hash_map.end()) {
            if (iter->second.vec == v) {
                // vectors are the same
                return iter->second.score_improvement;
            }
            return -1;
        }
        return -1;
    }

    /**
     * Checks if a two-element entry with the specified values exists in the cache.
     *
     * @param x The value of the first element.
     * @param y The value of the second element.
     * @return True if the entry exists and matches the provided values; false otherwise.
     */
    inline TypeSF get_entry_2D(size_t hash, size_t x, size_t y) {
        auto iter = hash_map_2D.find(hash);

        if (iter != hash_map_2D.end()) {
            if (iter->second.x1 == x && iter->second.x2 == y) {
                // vectors are the same
                return iter->second.score_improvement;
            }
            return -1;
        }
        return -1;
    }

    /**
     * Inserts an entry into the cache.
     *
     * @param v A vector of indices or values associated with the entry.
     * @param si The score improvement to be stored with the entry.
     */
    inline void insert_entry(size_t hash, std::vector<size_t> &v, TypeSF si) {
        hash_map.emplace(hash, CacheEntry(v, si));
    }

    /**
     * Inserts a two-element entry into the cache.
     *
     * @param x The value of the first element.
     * @param y The value of the second element.
     * @param si The score improvement to be stored with the entry.
     */
    inline void insert_entry_2D(size_t hash, size_t x, size_t y, TypeSF si) {
        hash_map_2D.emplace(hash, CacheEntry2D(x, y, si));
    }

    /**
     * Clears the cache when transitioning to a new depth.
     */
    inline void visit_new_depth() {
        clear();
    }

    /**
     * Calculates the hash value for a vector of size_t values.
     *
     * @param vec A vector of size_t values to calculate the hash for.
     * @return The calculated hash value.
     */
    inline static size_t vector_hash(std::vector<size_t> &vec) {
        // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
        std::size_t seed = vec.size();
        for (auto &i: vec) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    /**
     * Calculates the hash value for two size_t values (2D case).
     *
     * @param x The value of the first element.
     * @param y The value of the second element.
     * @return The calculated hash value.
     */
    inline static size_t hash_2D(size_t x, size_t y) {
        // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
        std::size_t seed = 2;
        seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= y + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};


#endif //SMSM_SICACHE_H
