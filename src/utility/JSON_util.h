#ifndef SMSM_JSON_UTIL_H
#define SMSM_JSON_UTIL_H

#include <cstddef>
#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "util.h"

std::string to_JSON_key(bool b);

std::string to_JSON_key(double d);

std::string to_JSON_key(uint32_t n);

std::string to_JSON_key(size_t n);

std::string to_JSON_key(int n);

std::string to_JSON_key(const std::string &s);

std::string to_JSON_value(bool b);

std::string to_JSON_value(double d);

std::string to_JSON_value(uint32_t n);

std::string to_JSON_value(size_t n);

std::string to_JSON_value(int n);

template<typename T>
std::string to_JSON_value(std::vector<T> &v) {
    std::string content = "[";

    if (v.empty()) {
        content += "]";
        return content;
    }

    for (auto entry: v) {
        content += to_JSON_value(entry) + ", ";
    }
    content.pop_back();
    content.back() = ']';

    return content;
}

std::string to_JSON_value(const std::string &s);

template<typename T1, typename T2>
std::string to_JSON(const std::map<T1, T2> &m) {
    std::string content = "{";

    if (m.empty()) {
        content += "}";
        return content;
    }

    for (auto entry: m) {
        content += to_JSON_key(entry.first) + " : " + to_JSON_value(entry.second) + ",\n";
    }
    content.pop_back();
    content.back() = '}';

    return content;
}

template<typename T>
std::string to_JSON(const std::vector<T> &v) {
    std::string content = "[";

    if (v.empty()) {
        content += "]";
        return content;
    }

    for (auto entry: v) {
        content += to_JSON_value(entry) + ", ";
    }
    content.pop_back();
    content.back() = ']';

    return content;
}

template<typename T>
std::string to_JSON(T *arr, size_t size) {
    std::string content = "[";

    if (size == 0) {
        content += "]";
        return content;
    }

    for (size_t i = 0; i < size; ++i) {
        content += to_JSON_value(arr[i]) + ", ";
    }
    content.pop_back();
    content.back() = ']';

    return content;
}

/**
 * Reads one vector from a JSON file.
 *
 * @param file_path Path to the file.
 * @return The vector.
 */
std::vector<uint32_t> read_initial_vector(std::string &file_path);

#endif //SMSM_JSON_UTIL_H
