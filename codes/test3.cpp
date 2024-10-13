#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>
#include <random>
#include <sstream>

using namespace std;

// Функция MergeSort должна принимать итераторы категории произвольного доступа
template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    // Если размер диапазона меньше 2, то сортировать нечего
    if (distance(range_begin, range_end) < 2) {
        return;
    }

    // Находим середину диапазона
    RandomIt middle = range_begin + distance(range_begin, range_end) / 2;

    // Рекурсивно сортируем две части
    MergeSort(range_begin, middle);
    MergeSort(middle, range_end);

    // Вектор для временного хранения отсортированных данных
    vector<typename RandomIt::value_type> merged;

    // Сливаем две отсортированные части
    merge(range_begin, middle, middle, range_end, back_inserter(merged));

    // Копируем результат слияния обратно в исходный диапазон
    copy(merged.begin(), merged.end(), range_begin);
}

// Функция для вывода диапазона
template <typename It>
string PrintRange(It range_begin, It range_end) {
    ostringstream out;
    for (auto it = range_begin; it != range_end; ++it) {
        out << *it << " ";
    }
    return out.str();
}

int main() {
    vector<int> test_vector(10);
    // Заполняем вектор последовательными числами от 1 до 10
    iota(test_vector.begin(), test_vector.end(), 1);

    // Перемешиваем элементы в случайном порядке
    random_device rd;
    mt19937 g(rd());
    shuffle(test_vector.begin(), test_vector.end(), g);
    
    // Выводим вектор до сортировки
    cout << "Before sort: " << PrintRange(test_vector.begin(), test_vector.end()) << endl;

    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());

    // Выводим результат
    cout << "After sort: " << PrintRange(test_vector.begin(), test_vector.end()) << endl;
    
    return 0;
}
