#include <algorithm>
#include <iostream>
#include <functional>
#include <limits>
#include <vector>
#include <numeric>
#include <cassert>
#include <stdexcept>

// Универсальная функция подсчёта, принимающая вектор по константной ссылке
int AllCount(const std::vector<int>& vec, std::function<bool(int)> condition) {
    return std::count_if(vec.begin(), vec.end(), condition);
}

// Функция подсчёта минимального и максимального введённого числа
std::pair<int, int> MaxMinCount(const std::vector<int>& vec) {
    if (vec.empty()) {
        throw std::domain_error("Vector is empty");
    }
    auto minMax = std::minmax_element(vec.begin(), vec.end());
    return { *minMax.first, *minMax.second };
}

// Функция подсчёта среднего арифметического 
double Average(const std::vector<int>& vec) {
    if (vec.empty()) {
        throw std::domain_error("Cannot calculate average of empty vector");
    }
    double result = std::accumulate(vec.begin(), vec.end(), 0);
    return result / vec.size();
}

// Функция медианы введённых чисел
double CalculateMedian(std::vector<int> vec) { // Сортируем копию вектора
    std::sort(vec.begin(), vec.end());
    size_t size = vec.size();
    if (size == 0) {
        throw std::domain_error("Median cannot be calculated on empty vector");
    }
    if (size % 2 == 0) {
        return (vec[size / 2 - 1] + vec[size / 2]) / 2.0;
    } else {
        return vec[size / 2];
    }
}

void PrintResult(int r1, int r2, int r3, const std::pair<int, int>& maxMinCount, double average, double median) {
    std::cout << "Count of '5': " << r1 << std::endl;
    std::cout << "Count of numbers more than '3': " << r2 << std::endl;
    std::cout << "Count of numbers in range from '2' to '8': " << r3 << std::endl;
    std::cout << "Min: " << maxMinCount.first << ", Max: " << maxMinCount.second << std::endl;
    std::cout << "Average: " << average << std::endl;
    std::cout << "Median: " << median << std::endl;
}

void RunTests() {
    std::vector<int> vec = {1, 5, 3, 7, 5, 8, 2, 6, 5, 9};

    // Test AllCount
    assert(AllCount(vec, [](int num) { return num == 5; }) == 3);
    assert(AllCount(vec, [](int num) { return num > 3; }) == 7);
    assert(AllCount(vec, [](int num) { return num >= 2 && num <= 8; }) == 8);

    // Test MaxMinCount
    auto result = MaxMinCount(vec);
    assert(result.first == 1);
    assert(result.second == 9);

    // Test Average
    assert(Average(vec) == 5.1);

    // Test CalculateMedian
    assert(CalculateMedian(vec) == 5.0);

    // Test exception handling for empty vector
    std::vector<int> emptyVec;
    try {
        MaxMinCount(emptyVec);
        assert(false); // Ошибка, если исключение не было выброшено
    } catch (const std::domain_error&) {
        assert(true); // Исключение успешно перехвачено
    }

    try {
        Average(emptyVec);
        assert(false); // Ошибка, если исключение не было выброшено
    } catch (const std::domain_error&) {
        assert(true); // Исключение успешно перехвачено
    }

    try {
        CalculateMedian(emptyVec);
        assert(false); // Ошибка, если исключение не было выброшено
    } catch (const std::domain_error&) {
        assert(true); // Исключение успешно перехвачено
    }

    std::cout << "All tests passed!" << std::endl;
}

int main() {
    RunTests(); // Вызов тестов

    std::vector<int> counter;
    int x;

    std::cout << "Insert numbers. To stop, insert a negative number." << std::endl;

    while (true) {
        std::cin >> x;

        // Прерывание ввода при отрицательном числе
        if (x < 0) {
            break;
        }

        // Обработка некорректного ввода
        if (std::cin.fail()) {
            std::cin.clear(); // Очистка флага ошибки
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорирование некорректного ввода
            std::cout << "Error, enter only natural numbers!" << std::endl;
        } else {
            counter.push_back(x);
        }

        // Ограничение на количество чисел (максимум 100)
        if (counter.size() >= 100) {
            std::cout << "Limit of 100 numbers reached." << std::endl;
            break;
        }
    }

    // Использование универсальной функции для различных подсчетов
    auto r1 = AllCount(counter, [](int num) { return num == 5; });
    auto r2 = AllCount(counter, [](int num) { return num > 3; });
    auto r3 = AllCount(counter, [](int num) { return num >= 2 && num <= 8; });

    // Вывод результатов
    std::pair<int, int> maxMinCount;
    double average, median;

    try {
        maxMinCount = MaxMinCount(counter);
        average = Average(counter);
        median = CalculateMedian(counter);
    } catch (const std::domain_error& e) {
        std::cout << e.what() << std::endl;
        return 1; // Завершение программы при ошибке
    }

    // Вызов функции Print для вывода результатов
    PrintResult(r1, r2, r3, maxMinCount, average, median);

    // Вывод всех введённых чисел, разделённых запятыми
    std::cout << "Numbers: ";
    for (size_t i = 0; i < counter.size(); i++) {
        std::cout << counter[i];
        if (i != counter.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;

    return 0;
}
