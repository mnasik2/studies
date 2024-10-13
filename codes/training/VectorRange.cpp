#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <cassert>  // Подключаем assert

using namespace std;

struct Lang {
    string language;
    int age;
};

// Перегрузка оператора вывода для структуры Lang
ostream& operator<<(ostream& os, const Lang& lang) {
    os << lang.language  << " " << lang.age << " ";
    return os;
}

// Шаблонная функция для вывода диапазона элементов
template <typename It>
void PrintRange(It range_begin, It range_end) {
    if (range_begin == range_end) {
        cout << "Контейнер пуст." << endl;
        return;
    }

    for (It it = range_begin; it != range_end; ++it) {
        cout << *it << " ";
    }
    cout << endl;  // Перевод строки после вывода всех элементов
}

template <typename Container, typename Element>
void FindAndPrint(const Container& container, const Element& elem) {
    auto it = find(container.begin(), container.end(), elem);

    PrintRange(container.begin(), it);

    PrintRange(it, container.end());
}

#define TEST_EQUAL(a, b) assert((a) == (b))  // Макрос для проверки равенства

int main() {
    vector<Lang> langs = {{"Python", 29}, {"Java", 24}, {"C#", 20}, {"Ruby", 25}, {"C++", 37}};
    
    // Тест 1: множество с целыми числами
    set<int> test1 = {1, 1, 1, 2, 3, 4, 5, 5};
    cout << "Test1"s << endl;
    PrintRange(test1.begin(), test1.end());

    // Проверка длины множества
    TEST_EQUAL(test1.size(), 5);  // Должно быть 5 уникальных элементов

    // Тест 2: пустой вектор
    vector<int> test2 = {};
    cout << "Test2"s << endl;
    PrintRange(test2.begin(), test2.end());

    // Проверка на пустоту
    TEST_EQUAL(test2.empty(), true);

    // Тест 3: вектор структур Lang
    cout << "Test3"s << endl;
    PrintRange(langs.begin(), langs.end());

    // Проверка первого элемента
    TEST_EQUAL(langs[0].language, "Python");
    TEST_EQUAL(langs[0].age, 29);

    // Дополнительные тесты с функцией FindAndPrint
    set<int> test = {1, 1, 1, 2, 3, 4, 5, 5};
    cout << "Test4"s << endl;
    FindAndPrint(test, 3);  // Элемент 3 найден

    // Проверка на то, что элемент 3 существует
    assert(find(test.begin(), test.end(), 3) != test.end());

    cout << "Test5"s << endl;
    FindAndPrint(test, 0);  // Элемент 0 не найден

    // Проверка на то, что элемента 0 нет
    assert(find(test.begin(), test.end(), 0) == test.end());

    cout << "End of tests"s << endl;
    return 0;
}
