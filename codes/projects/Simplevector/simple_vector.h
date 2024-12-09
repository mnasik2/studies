#pragma once

#include "array_ptr.h"

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <cassert>
#include <utility>

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size)
        : items_(size), size_(size), capacity_(size) {}

    SimpleVector(size_t size, const Type& value)
        : items_(size), size_(size), capacity_(size) {
        std::fill(items_.Get(), items_.Get() + size_, value);
    }

    SimpleVector(std::initializer_list<Type> init)
        : items_(init.size()), size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), items_.Get());
    }

    SimpleVector(const SimpleVector& other)
        : items_(other.size_), size_(other.size_), capacity_(other.size_) {
        std::copy(other.items_.Get(), other.items_.Get() + size_, items_.Get());
    }

    SimpleVector& operator=(const SimpleVector& other) {
        if (this != &other) {
            SimpleVector tmp(other);
            swap(tmp);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other) noexcept
        : items_(std::move(other.items_)),
          size_(std::exchange(other.size_, 0)),
          capacity_(std::exchange(other.capacity_, 0)) {}

    SimpleVector& operator=(SimpleVector&& other) noexcept {
        if (this != &other) {
            items_ = std::move(other.items_);
            size_ = std::exchange(other.size_, 0);
            capacity_ = std::exchange(other.capacity_, 0);
        }
        return *this;
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void PopBack() noexcept {
        if (size_ > 0) {
            --size_;
        }
    }

    void PushBack(Type&& value) {
        if (size_ == capacity_) {
            Reserve(capacity_ == 0 ? 1 : 2 * capacity_);
        }
        items_[size_] = std::move(value);
        ++size_;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> new_items(new_capacity);
            std::move(items_.Get(), items_.Get() + size_, new_items.Get());
            items_.swap(new_items);
            capacity_ = new_capacity;
        }
    }

    Iterator begin() noexcept {
        return items_.Get();
    }

    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    Iterator Erase(ConstIterator pos) { 
    assert(pos >= begin() && pos < end() && "Iterator out of range"); 
    size_t index = pos - begin(); 
    for (size_t i = index; i < size_ - 1; ++i) { 
    items_[i] = std::move(items_[i + 1]);   
    } 
    --size_; 
    return begin() + index; 

    } 
    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end() && "Iterator out of range");

        size_t index = pos - begin();

        if (size_ < capacity_) {
            std::move_backward(items_.Get() + index, items_.Get() + size_, items_.Get() + size_ + 1);
            items_[index] = std::move(value);
            ++size_;
        } else {
            size_t new_capacity = (capacity_ == 0) ? 1 : 2 * capacity_;
            ArrayPtr<Type> new_items(new_capacity);
            std::move(items_.Get(), items_.Get() + index, new_items.Get());
            new_items[index] = std::move(value);
            std::move(items_.Get() + index, items_.Get() + size_, new_items.Get() + index + 1);
            items_.swap(new_items);
            capacity_ = new_capacity;
            ++size_;
        }

        return begin() + index;
    }

    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
