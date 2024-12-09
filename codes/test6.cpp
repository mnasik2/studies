#include <algorithm>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

template <typename Token>
using Sentence = vector<Token>;

template <typename TokenForwardIt>
TokenForwardIt FindSentenceEnd(TokenForwardIt tokens_begin, TokenForwardIt tokens_end) {
    const TokenForwardIt before_sentence_end
        = adjacent_find(tokens_begin, tokens_end, [](const auto& left_token, const auto& right_token) {
              return left_token.IsEndSentencePunctuation() && !right_token.IsEndSentencePunctuation();
          });
    return before_sentence_end == tokens_end ? tokens_end : next(before_sentence_end);
}

template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> result;
    auto sentence_begin = tokens.begin();
    while (sentence_begin != tokens.end()) {
        auto sentence_end = FindSentenceEnd(sentence_begin, tokens.end());
        result.push_back(Sentence<Token>(
            make_move_iterator(sentence_begin),
            make_move_iterator(sentence_end)
        ));
        sentence_begin = sentence_end;
    }
    return result;
}

struct TestToken {
    string data;
    bool is_end_sentence_punctuation = false;

    static int copy_count;
    static int move_count;

    TestToken() = default;
    TestToken(const string& data, bool is_end_sentence_punctuation = false)
        : data(data), is_end_sentence_punctuation(is_end_sentence_punctuation) {}

    bool IsEndSentencePunctuation() const {
        return is_end_sentence_punctuation;
    }

    bool operator==(const TestToken& other) const {
        return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
    }

    // Статические методы для подсчета копий и перемещений
    static void ResetCounts() {
        copy_count = 0;
        move_count = 0;
    }

    static void PrintCounts() {
        cout << "Copies: " << copy_count << ", Moves: " << move_count << endl;
    }

    // Конструктор копирования
    TestToken(const TestToken& other) : data(other.data), is_end_sentence_punctuation(other.is_end_sentence_punctuation) {
        ++copy_count;
    }

    // Конструктор перемещения
    TestToken(TestToken&& other) noexcept : data(std::move(other.data)), is_end_sentence_punctuation(other.is_end_sentence_punctuation) {
        ++move_count;
    }

    // Оператор присваивания копированием
    TestToken& operator=(const TestToken& other) {
        if (this != &other) {
            data = other.data;
            is_end_sentence_punctuation = other.is_end_sentence_punctuation;
            ++copy_count;
        }
        return *this;
    }

    // Оператор присваивания перемещением
    TestToken& operator=(TestToken&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            is_end_sentence_punctuation = other.is_end_sentence_punctuation;
            ++move_count;
        }
        return *this;
    }
};

int TestToken::copy_count = 0;
int TestToken::move_count = 0;

ostream& operator<<(ostream& stream, const TestToken& token) {
    return stream << token.data;
}

void TestTokenFunction(std::vector<TestToken>& tokens) {
    TestToken::ResetCounts();

    std::vector<TestToken> temp;
    for (auto&& item : tokens) {
        temp.push_back(std::move(item));
    }
    tokens = std::move(temp);

    TestToken::PrintCounts();
}

void TestSplitting() {
 
    vector<TestToken> tokens = {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}};
    TestTokenFunction(tokens);
    assert(SplitIntoSentences(tokens) == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}}}));

    tokens = {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}};
    TestTokenFunction(tokens);
    assert(SplitIntoSentences(tokens) == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}}}));

    tokens = {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}, {"Without"s}, {"copies"s}, {"."s, true}};
    TestTokenFunction(tokens);
    assert(SplitIntoSentences(tokens) == vector<Sentence<TestToken>>({
        {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}},
        {{"Without"s}, {"copies"s}, {"."s, true}},
    }));
}

int main() {
    TestSplitting();
}