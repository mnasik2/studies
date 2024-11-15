#include <algorithm>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <iostream>

using namespace std;

template <typename T>
class PtrVector {
public:
    PtrVector() = default;

    // Копирующий конструктор с резервированием места
    PtrVector(const PtrVector& other) {
        // Предварительно резервируем место для копируемых элементов
        items_.reserve(other.items_.size());

        for (const auto& ptr : other.items_) {
            if (ptr) {
                // Создаем копию объекта и добавляем указатель на нее
                items_.push_back(new T(*ptr));
            } else {
                // Если указатель nullptr, просто добавляем nullptr
                items_.push_back(nullptr);
            }
        }
    }

    // Деструктор. Удаляет объекты в куче, на которые ссылаются указатели,
    // в векторе items_
    ~PtrVector() {
        for (T* ptr : items_) {
            delete ptr;
        }
    }

    // Возвращает ссылку на вектор указателей (неконстантную)
    vector<T*>& GetItems() noexcept {
        return items_;
    }

    // Возвращает константную ссылку на вектор указателей
    const vector<T*>& GetItems() const noexcept {
        return items_;
    }

private:
    vector<T*> items_;
};

// Эта функция main тестирует шаблон класса PtrVector
int main() {
    // Вспомогательный "шпион", позволяющий узнать о своём удалении
    struct DeletionSpy {
        explicit DeletionSpy(bool& is_deleted)
            : is_deleted_(is_deleted) {
        }
        ~DeletionSpy() {
            is_deleted_ = true;
        }
        bool& is_deleted_;
    };

    // Проверяем удаление элементов
    {
        bool spy1_is_deleted = false;
        DeletionSpy* ptr1 = new DeletionSpy(spy1_is_deleted);
        {
            PtrVector<DeletionSpy> ptr_vector;
            ptr_vector.GetItems().push_back(ptr1);
            assert(!spy1_is_deleted);

            // Константная ссылка на ptr_vector
            const PtrVector<DeletionSpy>& const_ptr_vector_ref = ptr_vector;
            // И константная, и неконстантная версия GetItems
            // должны вернуть ссылку на один и тот же вектор
            assert(&const_ptr_vector_ref.GetItems() == &ptr_vector.GetItems());
        }
        // При разрушении ptr_vector должен удалить все объекты, на которые
        // ссылаются находящиеся внутри него указатели
        assert(spy1_is_deleted);
    }

    // Вспомогательный «шпион», позволяющий узнать о своём копировании
    struct CopyingSpy {
        explicit CopyingSpy(int& copy_count)
            : copy_count_(copy_count) {
        }
        CopyingSpy(const CopyingSpy& rhs)
            : copy_count_(rhs.copy_count_) {
            ++copy_count_;
        }
        int& copy_count_;
    };

    // Проверяем копирование элементов при копировании массива указателей
    {
        // 10 элементов
        vector<int> copy_counters(10, 0);

        PtrVector<CopyingSpy> ptr_vector;
        // Подготавливаем оригинальный массив указателей
        for (auto& counter : copy_counters) {
            ptr_vector.GetItems().push_back(new CopyingSpy(counter));
        }
        // Последний элемент содержит нулевой указатель
        ptr_vector.GetItems().push_back(nullptr);

        // Создаем копию ptr_vector
        auto ptr_vector_copy(ptr_vector);
        // Количество элементов в копии равно количеству элементов оригинального вектора
        assert(ptr_vector_copy.GetItems().size() == ptr_vector.GetItems().size());

        // Копия должна хранить указатели на новые объекты
        // Проверяем, что указатели разные
        for (size_t i = 0; i < ptr_vector.GetItems().size(); ++i) {
            if (ptr_vector.GetItems()[i] != nullptr) {
                assert(ptr_vector_copy.GetItems()[i] != ptr_vector.GetItems()[i]);
            } else {
                assert(ptr_vector_copy.GetItems()[i] == nullptr);
            }
        }

        // Последний элемент исходного массива и его копии - нулевой указатель
        assert(ptr_vector_copy.GetItems().back() == nullptr);

        // Проверяем, что элементы были скопированы (копирующие шпионы увеличивают счётчики копий).
        for (auto counter : copy_counters) {
            assert(counter == 1);
        }

        // Очистка памяти в копии
        // Деструктор PtrVector удалит все объекты
    }

    // Дополнительный тест: пустой PtrVector
    {
        PtrVector<int> empty_ptr_vector;
        PtrVector<int> empty_ptr_vector_copy(empty_ptr_vector);
        assert(empty_ptr_vector_copy.GetItems().empty());
    }

    cout << "All tests passed successfully!" << endl;
}
