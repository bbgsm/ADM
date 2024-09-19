#pragma once

#include <iostream>
#include <assert.h>

template<typename T>
class c_memory_region {
public:
    c_memory_region() : region_start(0), size(0) {
    }

    c_memory_region(T user_object, uintptr_t region_start, size_t size)
            : user_object(user_object), region_start(region_start), size(size) {
    }

    T get_object() const {
        return user_object;
    }

    uintptr_t get_region_start() const {
        return region_start;
    }

    uintptr_t get_region_end() const {
        return region_start + size - 1;
    }

    size_t get_region_size() const {
        return size;
    }

    bool operator<(const c_memory_region &other) const {
        return this->region_start < other.region_start;
    }

    bool operator==(const c_memory_region &other) const {
        return this->region_start == other.region_start;
    }

    bool contains(uintptr_t address) const {
        return address >= get_region_start() && address < get_region_end() + 1;
    }

private:
    T user_object;
    uintptr_t region_start;
    size_t size;
};
