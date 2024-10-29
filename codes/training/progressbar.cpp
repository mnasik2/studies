#include <iostream>
#include <chrono>
#include <thread>

void showProgressBar(int duration) {
    const int barWidth = 70; // Ширина прогресс-бара

    for (int i = 0; i <= duration; ++i) {
        // Вычисляем процент завершения
        float progress = static_cast<float>(i) / duration;
        
        // Вычисляем количество заполненных символов
        int pos = barWidth * progress;

        // Отображаем прогресс-бар
        std::cout << "\r[";
        for (int j = 0; j < barWidth; ++j) {
            if (j < pos) {
                std::cout << "◼︎"; // Заполненный символ
            } else {
                std::cout << " "; // Пустой символ
            }
        }
        std::cout << "] " << static_cast<int>(progress * 100) << "%";
        std::cout.flush(); // Принудительно очищаем вывод

        // Задержка для имитации загрузки
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Задержка в 100 мс
    }
    std::cout << std::endl; // Перенос строки после завершения
}

int main() {
    int duration = 100; // Продолжительность загрузки (время выполнения прогресс-бара)
    showProgressBar(duration);
    return 0;
}
