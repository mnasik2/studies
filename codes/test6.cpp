#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

// Цель этой функции — создать все файлы с нужным содержимым.
void CreateFiles(const string& config_file) {
    // Открываем конфигурационный файл для чтения
    ifstream in_file(config_file);
    if (!in_file) {
        cerr << "Не удалось открыть файл конфигурации: " << config_file << endl;
        return;
    }

    string line;
    ofstream out_file;
    
    // Чтение конфигурационного файла построчно
    while (getline(in_file, line)) {
        if (!line.empty() && line[0] != '>') {
            // Если строка не начинается с '>', это имя файла — закрываем старый файл и открываем новый
            if (out_file.is_open()) {
                out_file.close();
            }
            out_file.open(line);
            if (!out_file) {
                cerr << "Не удалось открыть файл для записи: " << line << endl;
                return;
            }
        } else if (!line.empty() && line[0] == '>') {
            // Если строка начинается с '>', записываем содержимое в текущий файл
            if (out_file.is_open()) {
                out_file << line.substr(1) << endl;
            } else {
                cerr << "Нет открытого файла для записи" << endl;
                return;
            }
        }
    }

    // Закрываем последний открытый файл
    if (out_file.is_open()) {
        out_file.close();
    }
}

string GetLine(istream& in) {
    string s;
    getline(in, s);
    return s;
}

int main() {
    ofstream("test_config.txt"s) << "a.txt\n"
                                    ">10\n"
                                    ">abc\n"
                                    "b.txt\n"
                                    ">123"s;

    CreateFiles("test_config.txt"s);
    ifstream in_a("a.txt"s);
    assert(GetLine(in_a) == "10"s && GetLine(in_a) == "abc"s);

    ifstream in_b("b.txt"s);
    assert(GetLine(in_b) == "123"s);
}