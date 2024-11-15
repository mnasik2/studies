#include <cassert>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace std;

// Умный указатель, удаляющий связанный объект при своём разрушении.
template <typename T>
class ScopedPtr {
public:
    ScopedPtr() = default;

    explicit ScopedPtr(T* raw_ptr) noexcept : ptr_(raw_ptr) {}

    ScopedPtr(const ScopedPtr&) = delete;

    ~ScopedPtr() {
        delete ptr_;
    }

    T* GetRawPtr() const noexcept {
        return ptr_;
    }

    T* Release() noexcept {
        T* old_ptr = ptr_;
        ptr_ = nullptr;
        return old_ptr;
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    T& operator*() const {
        if (ptr_ == nullptr) {
            throw std::logic_error("Dereferencing null pointer");
        }
        return *ptr_;
    }

    T* operator->() const {
        if (ptr_ == nullptr) {
            throw std::logic_error("Accessing member of null pointer");
        }
        return ptr_;
    }

private:
    T* ptr_ = nullptr;
};

int main() {
    // Проверка работы оператора приведения к типу bool
    {
        const ScopedPtr<int> empty_ptr;
        assert(!empty_ptr);

        const ScopedPtr<int> ptr_to_existing_object(new int(0));
        assert(ptr_to_existing_object);

        static_assert(noexcept(static_cast<bool>(ptr_to_existing_object)));
    }

    // Проверка работы оператора разыменования *
    {
        string* raw_ptr = new string("hello");
        ScopedPtr<string> smart_ptr(raw_ptr);
        assert(&*smart_ptr == raw_ptr);

        try {
            ScopedPtr<int> empty_ptr;
            *empty_ptr;
            assert(false);
        } catch (const logic_error&) {
        } catch (...) {
            assert(false);
        }
    }

    // Проверка работы оператора ->
    {
        string* raw_ptr = new string("hello");
        ScopedPtr<string> smart_ptr(raw_ptr);
        assert(smart_ptr->data() == raw_ptr->data());

        try {
            ScopedPtr<string> empty_ptr;
            empty_ptr->clear();
            assert(false);
        } catch (const logic_error&) {
        } catch (...) {
            assert(false);
        }
    }

    // Пример использования
    {
        struct Object {
            Object() {
                std::cout << "Object is default constructed"s << std::endl;
            }
            void DoSomething() {
                std::cout << "Doing something"s << std::endl;
            }
            ~Object() {
                std::cout << "Object is destroyed"s << std::endl;
            }
        };

        ScopedPtr<Object> empty_smart_ptr;
        assert(!empty_smart_ptr);

        ScopedPtr<Object> smart_ptr(new Object());
        assert(smart_ptr);

        (*smart_ptr).DoSomething();
        smart_ptr->DoSomething();
    }
}
