#include "workwithfiles.h"


using namespace std;
namespace fs = std::filesystem;

// Реализация функций

void CreateFiles(const string& file) {
    ofstream out_file(file);
    if (!out_file) {
        cerr << "Не удалось создать файл: " << file << endl;
    }
}

void CopyFile(const string& source, const string& destination) {
    try {
        if (fs::exists(source) && fs::is_regular_file(source)) {
            if (fs::exists(destination)) {
                cout << "Файл уже существует в месте назначения: " << destination << endl;
            } else {
                fs::copy_file(source, destination);
                cout << "Файл успешно скопирован в: " << destination << endl;
            }
        } else {
            cerr << "Исходный файл не найден или не является обычным файлом." << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Ошибка при копировании файла: " << e.what() << endl;
    }
}

void CurrentPath() {
    try {
        cout << "Текущий рабочий каталог: " << fs::current_path() << endl;
    } catch (const fs::filesystem_error& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void FileSize(const string& file) {
    try {
        if (fs::exists(file) && fs::is_regular_file(file)) {
            cout << "Размер файла: " << fs::file_size(file) << " байт" << endl;
        } else {
            cerr << "Файл не найден или не является обычным файлом: " << file << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Ошибка при чтении размера файла: " << e.what() << endl;
    }
}

void RemoveFile(const string& file) {
    try {
        if (fs::exists(file)) {
            if (fs::is_regular_file(file)) {
                fs::remove(file);
                cout << "Файл " << file << " успешно удалён." << endl;
            } else {
                cerr << "Указанный путь " << file << " существует, но не является обычным файлом." << endl;
            }
        } else {
            cerr << "Файл " << file << " не найден." << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Ошибка при удалении файла: " << e.what() << endl;
    }
}

void ResizeFile(const string& file, uintmax_t new_size) {
    try {
        if (fs::exists(file)) {
            if (fs::is_regular_file(file)) {
                fs::resize_file(file, new_size);
                cout << "Размер файла " << file << " успешно изменён на " << new_size << " байт." << endl;
            } else {
                cerr << "Не удалось изменить размер файла " << file << ", он существует, но не является обычным файлом." << endl;
            }
        } else {
            cerr << "Файл " << file << " не найден." << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Ошибка при изменении размера файла: " << e.what() << endl;
    }
}

int main() {
    //SetConsoleOutputCP(CP_UTF8);
    CreateFiles("a_file");
    CopyFile("a_file", "a_file_back");
    RemoveFile("a_file_back");

    CurrentPath();
    FileSize("numbers.txt");
    ResizeFile("numbers.txt", 128);
    FileSize("numbers.txt");

    return 0;
}
