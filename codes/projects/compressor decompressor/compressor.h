#pragma once

// Этот файл содержит компрессор RLE, не меняйте его.
// Можете изучить устройство компрессора, чтобы лучше понять формат сжатого файла.
// Сжатие и разжатие должно давать исходный файл

#include <fstream>
#include <iostream>
#include <string>

// Класс CompressorRLE отвечает за сжатие данных с использованием алгоритма RLE (Run-Length Encoding)
class CompressorRLE {
public:
    // Максимальный размер блока данных
    static const int max_block_size = 128;
    // Минимальное количество повторов, необходимое для специального блока
    static const int min_repeats_for_special_block = 3;

    // Конструктор класса, принимает поток назначения для записи сжатых данных
    CompressorRLE(std::ostream& dst)
        : dst_(dst) {  // Инициализируем поток назначения
    }

    // Метод для добавления символа в текущий блок
    void PutChar(char c) {
        // Если текущий блок не пуст и добавляемый символ равен последнему, увеличиваем счетчик повторов
        if (block_size_ > 0 && c == last_char_) {
            ++repeat_count_;
            // Если количество повторов достигло максимума, финализируем повторы
            if (repeat_count_ >= max_block_size - 1) {
                FinalizeRepeats();
            }
            return;  // Выходим из функции, если добавление прошло успешно
        }

        // Завершаем повторы, если символ другой
        FinalizeRepeats();
        AddCharToBlock(c);  // Добавляем символ в блок
    }

    // Финализируем сжатие, вызываем метод для записи оставшихся данных
    void Finalize() {
        FinalizeRepeats();  // Завершаем повторы перед завершением
        WriteBlock0(block, block_size_);  // Записываем последний блок данных
    }

    // Метод для получения размера сжатых данных
    size_t GetCompressedSize() const {
        return compressed_size_;  // Возвращаем размер сжатых данных
    }

private:
    // Метод для завершения повторов символа
    void FinalizeRepeats() {
        if (repeat_count_ == 0) {  // Если нет повторов, ничего не делаем
            return;
        }

        // Если недостаточно повторов для специального блока
        if (repeat_count_ < min_repeats_for_special_block) {
            for (; repeat_count_ > 0; --repeat_count_) {
                AddCharToBlock(last_char_);  // Добавляем символы по одному
            }
            return;  // Завершаем метод
        }

        // Если достаточно повторов, записываем блок
        WriteBlock0(block, block_size_ - 1);  // Записываем данные блока
        WriteBlock1(last_char_, repeat_count_ + 1);  // Записываем специальные данные о повторе
        block_size_ = 0;  // Сбрасываем размер блока
        repeat_count_ = 0;  // Сбрасываем счетчик повторов
    };

    // Метод для добавления символа в блок
    void AddCharToBlock(char c) {
        block[block_size_++] = c;  // Добавляем символ в блок
        if (block_size_ >= max_block_size) {  // Если блок заполнен
            WriteBlock0(block, block_size_);  // Записываем блок
            block_size_ = 0;  // Сбрасываем размер блока
        }
        last_char_ = c;  // Обновляем последний добавленный символ
    }

    // Метод для записи стандартного блока (блок с обычными данными)
    void WriteBlock0(char* data, int size) {
        if (size == 0) {  // Если размер блока равен нулю, ничего не делаем
            return;
        }

        // Формируем заголовок для блока, где size - 1 в битах
        unsigned char zero = static_cast<unsigned char>(((size - 1) << 1) + 0);
        dst_.put(zero);  // Записываем заголовок в поток
        dst_.write(data, size);  // Записываем данные блока

        compressed_size_ += 1 + static_cast<size_t>(size);  // Увеличиваем общий размер сжатых данных
    }

    // Метод для записи специального блока (блок с повторяющимся символом)
    void WriteBlock1(char data, int size) {
        if (size == 0) {  // Если размер блока равен нулю, ничего не делаем
            return;
        }

        // Формируем заголовок для специального блока, где size - 1 в битах
        unsigned char zero = static_cast<unsigned char>(((size - 1) << 1) + 1);
        dst_.put(zero);  // Записываем заголовок в поток
        dst_.put(data);  // Записываем повторяющийся символ

        compressed_size_ += 2;  // Увеличиваем общий размер сжатых данных на 2 (заголовок и символ)
    }

private:
    std::ostream& dst_;  // Поток назначения для записи сжатых данных

    size_t compressed_size_ = 0;  // Общий размер сжатых данных

    int block_size_ = 0;  // Текущий размер блока
    int repeat_count_ = 0;  // Количество повторов символа
    char last_char_;  // Последний добавленный символ

    char block[max_block_size];  // Массив для хранения данных блока
};

// Структура для хранения результата кодирования
struct EncodingResult {
    bool opened;  // Успешно ли открыт файл
    size_t src_size;  // Размер исходных данных
    size_t dst_size;  // Размер сжатых данных
};

// Функция для кодирования RLE
inline EncodingResult EncodeRLE(std::string src_name, std::string dst_name) {
    using namespace std;

    // Открываем входной файл
    ifstream in(src_name, ios::binary);
    if (!in) {  // Если файл не удалось открыть
        return {false, 0, 0};  // Возвращаем неудачный результат
    }

    // Открываем выходной файл для записи
    ofstream out(dst_name, ios::binary);

    CompressorRLE compressor(out);  // Создаем экземпляр компрессора
    size_t source_size = 0;  // Инициализируем размер исходных данных

    // Читаем данные из входного файла по блокам
    do {
        char buff[1024];  // Буфер для чтения данных
        in.read(buff, sizeof buff);  // Читаем данные в буфер
        size_t read = in.gcount();  // Получаем количество прочитанных байт
        source_size += read;  // Увеличиваем размер исходных данных

        // Обрабатываем каждый символ из прочитанного
        for (size_t i = 0; i < read; ++i) {
            compressor.PutChar(buff[i]);  // Передаем символ компрессору
        }
    } while (in);  // Повторяем, пока есть данные для чтения

    compressor.Finalize();  // Финализируем сжатие

    // Возвращаем результат сжатия
    return {true, source_size, compressor.GetCompressedSize()};
}
