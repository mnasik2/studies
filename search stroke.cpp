#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

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

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            if (!word.empty()) {
            stop_words_.insert(word);
        }
    }
 }

    void AddDocument(int document_id, const string& document) {
    const vector<string> words = SplitIntoWordsNoStop(document);
           if (words.empty()) {
            return;
        }
    const double word_count = static_cast<double>(words.size()); // кол-во слов в векторе
    for (const string& word : words) {
        word_to_document_freqs_[word][document_id] += 1.0 / word_count; //Встретили слово, разделяем сколько раз его встретили, на общее число слов, получаем словарь слов, у каждого из них ID и TF
    }
    ++document_count_; // идём по документам
}

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });

        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    int document_count_ = 0;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
        Query query_words;
        for (const string& word : SplitIntoWords(text)) {
            if (!word.empty()) {
                if (word[0] == '-') {
                    query_words.minus_words.insert(word.substr(1)); // Убираем минус и добавляем в minus_words
                } else {
                    query_words.plus_words.insert(word); // Добавляем в plus_words
                }
            }
        }
        return query_words;
    }

       vector<Document> FindAllDocuments(const Query& query) const {
        map<int, double> document_to_relevance; //вторую переменную меняем на double

        // Обработка плюсовых слов
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) > 0) {
                const double idf = log(static_cast<double>(document_count_) / word_to_document_freqs_.at(word).size());
                for (const auto& [document_id, tf] : word_to_document_freqs_.at(word)) {
                    document_to_relevance[document_id] += tf * idf;
                }
            }
        }
        // Обработка минусовых слов
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) > 0) {
                for (const auto& [document_id, ignore] : word_to_document_freqs_.at(word)) { //второй элемент нам не нужен в учёте минус-слов
                    document_to_relevance.erase(document_id);
                }
            }
        }

        // теперь несём всё в вектор
        vector<Document> matched_documents;
        for (const auto& [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({document_id, relevance});
    }

    return matched_documents;
}
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = " << document_id << ", "
             << "relevance = " << relevance << " }" << endl;
    }
}