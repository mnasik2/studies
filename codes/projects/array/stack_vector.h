#pragma once

#include <array>
#include <stdexcept>
#include <iterator>
/*
У контейнера array есть итераторы. Используйте их.
Компилятор может определять возвращаемый тип.
Не забывайте про ограничения массивов: размер не может быть больше вместимости.
Генерируйте исключения в случае нарушения этого или других условий.
*/

template <typename T, size_t N>
class StackVector {
public:
    explicit StackVector(size_t a_size = 0)
            : size(a_size) {
        if (size > N) {
            using namespace std::literals;
            throw std::invalid_argument("Too much"s);
        }
    }

    T& operator[](size_t index){
        if(index >= size) {
            throw std::out_of_range("Index out of range");
    
    }
    return vec[index];
    }

    const T& operator[](size_t index) const{
        if(index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return vec[index];
    }
    //Методы begin/end должны возвращать итераторы на начало и текущий конец вектора. Их тип не указан, выберите его самостоятельно.
    auto begin() { return vec.begin(); }
    auto end() { return vec.begin() + size; }
    auto begin() const { return vec.begin(); }
    auto end() const { return vec.begin() + size; }
    //Метод Size должен возвращать текущее количество объектов в векторе.
    //Конструктор принимает размер вектора — по аналогии со стандартным вектором. 
    //Если аргумент конструктора больше вместимости вектора, конструктор должен выбрасывать исключение invalid_argument.
   size_t Size() const {
        return size;
    }

    //Метод Capacity должен возвращать вместимость вектора — то есть количество объектов, которое в него может поместиться.
    size_t Capacity() const {
        return N;
    }
    //Метод PushBack добавляет новый элемент в конец вектора. 
    //Если текущий размер вектора равен его вместимости, метод PushBack должен выбрасывать стандартное исключение overflow_error.
   void PushBack(const T& value) {
        if (size == N) {
            throw std::overflow_error("Vector is full");
        }
        vec[size] = value;
        ++size;
    }

    //Метод PopBack уменьшает размер вектора на один и возвращает элемент вектора, который был последним. 
    //Если вектор пуст, метод PopBack должен выбрасывать стандартное исключение underflow_error.
   T PopBack() {
        if (size == 0) {
            throw std::underflow_error("Vector is empty");
        }
        --size;
        return vec[size];
    }


private:
    std::array<T, N> vec;
    size_t size = 0;
};
