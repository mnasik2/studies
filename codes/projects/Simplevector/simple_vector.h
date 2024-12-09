#pragma once

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <utility>

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity) {
        capacity_ = capacity;
    }
    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;
    SimpleVector(ReserveProxyObj reserve_proxy_obj)
    : items_(reserve_proxy_obj.capacity_ ? new Type[reserve_proxy_obj.capacity_] : nullptr),
      size_(0),
      capacity_(reserve_proxy_obj.capacity_) {}
    
    SimpleVector(const SimpleVector& other)
        : items_(other.size_ ? new Type[other.size_] : nullptr),
          size_(other.size_),
          capacity_(other.capacity_) {
        std::copy(other.items_.get(), other.items_.get() + size_, items_.get());
    }

    
    SimpleVector& operator=(const SimpleVector& other) {
        if (this != &other) {
            items_.reset();
            size_ = other.size_;
            capacity_ = other.capacity_;
            items_ = std::make_unique<Type[]>(capacity_);
            std::copy(other.items_.get(), other.items_.get() + size_, items_.get());
        }
        return *this;
    }


    SimpleVector(SimpleVector&& other) noexcept
        : items_(std::move(other.items_)),
          size_(other.size_),
          capacity_(other.capacity_) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    SimpleVector& operator=(SimpleVector&& other) noexcept {
        if (this != &other) {
            items_ = std::move(other.items_);
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    explicit SimpleVector(size_t size)
        : items_(size ? new Type[size]() : nullptr),
          size_(size),
          capacity_(size) {}

    SimpleVector(size_t size, const Type& value)
        : items_(size ? new Type[size] : nullptr),
          size_(size),
          capacity_(size) {
        std::fill(items_.get(), items_.get() + size_, value);
    }

    SimpleVector(std::initializer_list<Type> init)
        : items_(init.size() ? new Type[init.size()] : nullptr),
          size_(init.size()),
          capacity_(init.size()) {
        std::copy(init.begin(), init.end(), items_.get());
    }

    ~SimpleVector() = default;

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

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return items_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return items_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void PopBack() noexcept {
        if (size_ != 0) {
            --size_;
        }
    }

  void PushBack(Type&& value) {
    if (size_ < capacity_) {
        items_[size_] = std::move(value);
        ++size_;
    } else {
        size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
        std::unique_ptr<Type[]> new_items(new Type[new_capacity]);
        std::move(items_.get(), items_.get() + size_, new_items.get());
        new_items[size_] = std::move(value);
        ++size_;
        items_ = std::move(new_items);
        capacity_ = new_capacity;
    }
}

    void Resize(size_t new_size) {
    if (new_size < size_) {
        size_ = new_size;
    } else if (new_size > capacity_) {
        std::unique_ptr<Type[]> new_items(new Type[new_size]);
        std::move(items_.get(), items_.get() + size_, new_items.get());
        for (size_t i = size_; i < new_size; ++i) {
            new_items.get()[i] = Type(); 
        }
        items_ = std::move(new_items);
        capacity_ = new_size;
        size_ = new_size;
    } else {
        size_ = new_size;
    }
}

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        std::unique_ptr<Type[]> new_items(new Type[new_capacity]);
        std::copy(items_.get(), items_.get() + size_, new_items.get());
        items_ = std::move(new_items);
        capacity_ = new_capacity;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(items_, other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    Iterator begin() noexcept {
        return size_ > 0 ? items_.get() : nullptr;
    }

    Iterator end() noexcept {
        return size_ > 0 ? items_.get() + size_ : nullptr;
    }

    ConstIterator begin() const noexcept {
        return size_ > 0 ? items_.get() : nullptr;
    }

    ConstIterator end() const noexcept {
        return size_ > 0 ? items_.get() + size_ : nullptr;
    }

    ConstIterator cbegin() const noexcept {
        return begin();
    }

    ConstIterator cend() const noexcept {
        return end();
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
        std::move_backward(items_.get() + index, items_.get() + size_, items_.get() + size_ + 1);
        items_[index] = std::move(value);
        ++size_;
    } else {
        size_t new_capacity = (capacity_ == 0) ? 1 : 2 * capacity_;
        std::unique_ptr<Type[]> new_items(new Type[new_capacity]);

        std::move(items_.get(), items_.get() + index, new_items.get());
        new_items[index] = std::move(value);
        std::move(items_.get() + index, items_.get() + size_, new_items.get() + index + 1);

        items_ = std::move(new_items);
        capacity_ = new_capacity;
        ++size_;
    }

    return begin() + index;
}

private:
    std::unique_ptr<Type[]> items_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](Type x, Type y) {return x == y; });
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](Type x, Type y) {return x < y; });
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