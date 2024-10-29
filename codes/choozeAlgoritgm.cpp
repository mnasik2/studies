#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>
#include "log_duration.h"

using namespace std;

int EffectiveCount(const vector<int>& v, int n, int i) {
    int64_t expected_count = static_cast<int64_t>(v.size()) * (i + 1) / (n + 1);  // Ожидаемое количество чисел
    auto it = v.end();

    // Выбираем алгоритм в зависимости от ожидаемого количества элементов
    if (expected_count <= log2(v.size())) {
        cout << "Using find_if" << endl;
        it = find_if(v.begin(), v.end(), [i](int x) { return x > i; });
    } else {
        cout << "Using upper_bound" << endl;
        it = upper_bound(v.begin(), v.end(), i);
    }

    // Возвращаем количество элементов, не превышающих i
    return distance(v.begin(), it);
}


int main() {
    static const int NUMBERS = 1'000'000;
    static const int MAX = 1'000'000'000;

    mt19937 r;
    uniform_int_distribution<int> uniform_dist(0, MAX);

    vector<int> nums;
    for (int i = 0; i < NUMBERS; ++i) {
        int random_number = uniform_dist(r);
        nums.push_back(random_number);
    }
    sort(nums.begin(), nums.end());

    int i;
    cout << "Enter a number to count elements less than or equal to it: ";
    cin >> i;
    int result = EffectiveCount(nums, MAX, i);
    LOG_DURATION("Итого заняло времени: "s);
    cout << "Total numbers before "s << i << ": "s << result << endl;
}