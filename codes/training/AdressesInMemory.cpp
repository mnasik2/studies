#include <iostream>
#include <vector>

using namespace std;

// Класс House позволяет добавлять стены и двери, а также выводить их ID и адреса в памяти
class House {
public:
    // Конструктор по умолчанию
    House() = default;

    // Метод для добавления стен
    vector<int> BuildWalls() {
        int x;
        cout << "Введите количество стен: ";
        cin >> x;

        vector<int> walls;
        for (int i = 0; i < x; ++i) {
            walls.push_back(i);
        }
        return walls;
    }

    // Метод для отображения адресов стен в памяти
    void DisplayWallAddresses(const vector<int>& walls) const {
        cout << "Адреса стен в памяти: ";
        for (const int& wall : walls) {
            cout << &wall << " ";
        }
        cout << endl;
    }

    // Метод для добавления дверей
    vector<int> BuildDoors() {
        int x;
        cout << "Введите количество дверей: ";
        cin >> x;

        vector<int> doors;
        for (int i = 0; i < x; ++i) {
            doors.push_back(i);
        }
        return doors;
    }

    // Метод для отображения адресов дверей в памяти
    void DisplayDoorAddresses(const vector<int>& doors) const {
        cout << "Адреса дверей в памяти: ";
        for (const int& door : doors) {
            cout << &door << " ";
        }
        cout << endl;
    }

private:
    int wall_ = 0;
    int door_ = 0;
};

int main() {
    House myHouse;

    // Создаем стены и отображаем их адреса в памяти
    vector<int> walls = myHouse.BuildWalls();
    myHouse.DisplayWallAddresses(walls);

    // Создаем двери и отображаем их адреса в памяти
    vector<int> doors = myHouse.BuildDoors();
    myHouse.DisplayDoorAddresses(doors);

    return 0;
}
