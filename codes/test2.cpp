#include <string_view> // Для использования string_view
#include <vector>
#include <string>
#include <iostream>
#include "log_duration.h"

using namespace std;

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}
/*Алгоритм будет выглядеть следующим образом:
Удалите начало из str до первого непробельного символа, воспользовавшись методом remove_prefix. Он уберёт из string_view указанное количество символов.
В цикле используйте метод find с одним параметром, чтобы найти номер позиции первого пробела.
Добавьте в результирующий вектор элемент string_view, полученный вызовом метода substr, где начальная позиция будет 0, а конечная — найденная позиция пробела или npos.
Сдвиньте начало str так, чтобы оно указывало на позицию за пробелом. 
Это можно сделать методом remove_prefix, передвигая начало str на указанное в аргументе количество позиций.
Условие цикла нужно изменить. Подумайте, каким именно оно должно быть.
*/
vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result;
    str.remove_prefix(min(str.find_first_not_of(" "), str.size()));
    
    while (!str.empty()) {
        // тут выполнен инвариант: str не начинается с пробела
        int64_t space = str.find(' ');

        // здесь можно избавиться от проверки на равенство npos
        // если space == npos, метод substr ограничит возвращаемый string_view концом строки
        result.push_back(str.substr(0, space));
        str.remove_prefix(min(str.find_first_not_of(" ", space), str.size()));
    }

    return result;
}

string GenerateText() {
    const int SIZE = 10000000;
    string text(SIZE, 'a');
    for (int i = 100; i < SIZE; i += 100) {
        text[i] = ' ';
    }
    return text;
}

int main() {
    const string text = GenerateText();
    {
        LOG_DURATION("string");
        const auto words = SplitIntoWords(text);
        cout << words[0] << "\n";
    }
    {
        LOG_DURATION("string_view");
        const auto words = SplitIntoWordsView(text);
        cout << words[0] << "\n";
    }

    return 0;
} 