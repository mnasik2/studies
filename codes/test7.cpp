#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "log_duration.h"

using namespace std;

void FillMatrixWithRandomNumbers(std::vector<std::vector<int>>& matrix, int N) {
    std::srand(static_cast<unsigned int>(std::time(0)));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = std::rand() % 100;
        }
    }
}

void PrintMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int elem : row) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
}
LOG_DURATION("time"s);
std::vector<int> Spiral(const vector<vector<int>>& matrix) {
    std::vector<int> result; 
    int top = 0;
    int bottom = matrix.size() - 1;
    int left = 0;
    int right = matrix[0].size() - 1;

    while (top <= bottom && left <= right) {
        // Проходим сверху вправо
        for (int i = left; i <= right; ++i) {
            result.push_back(matrix[top][i]);
        }
        ++top;  // Сужаем верхнюю границу

        // Проходим справа вниз
        for (int i = top; i <= bottom; ++i) {
            result.push_back(matrix[i][right]);
        }
        --right;  // Сужаем правую границу

        if (top <= bottom) {
            // Проходим снизу влево
            for (int i = right; i >= left; --i) {
                result.push_back(matrix[bottom][i]);
            }
            --bottom;  // Сужаем нижнюю границу
        }

        if (left <= right) {
            // Проходим слева вверх
            for (int i = bottom; i >= top; --i) {
                result.push_back(matrix[i][left]);
            }
            ++left;  // Сужаем левую границу
        }
    }

    return result;
}

void PrintSpiral(const std::vector<int>& spiral) {
    for (int num : spiral) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

int main() {
    int N = 5;  // Размер матрицы
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N));

    // Заполняем матрицу случайными числами
    FillMatrixWithRandomNumbers(matrix, N);

    std::cout << "Randomly filled matrix:" << std::endl;
    PrintMatrix(matrix);  // Выводим случайно заполненную матрицу

    // Получаем элементы в спиральном порядке
    std::vector<int> spiralOrder = Spiral(matrix);
    
    std::cout << "Spiral ordered matrix:" << std::endl;
    PrintSpiral(spiralOrder);  // Выводим элементы в спиральном порядке

    return 0;
}
