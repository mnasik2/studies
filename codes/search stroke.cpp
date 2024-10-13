#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <numeric>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
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
    Document() = default;

    Document(int id, double relevance, int rating)
        : id(id), relevance(relevance), rating(rating) {
    }

    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

ostream& operator<<(ostream& os, const Document& document) {
    os << "{ document_id = " << document.id 
       << ", relevance = " << document.relevance 
       << ", rating = " << document.rating << " }";
    return os;
}

template <typename StringContainer>
set<string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    set<string> non_empty_strings;
    for (const string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
public:
    inline static constexpr int INVALID_DOCUMENT_ID = -1;

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words) 
    : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {

        if (std::any_of(stop_words_.begin(), stop_words_.end(), 
                    [](const string& word) { return !IsValidWord(word); })) {
            throw invalid_argument("Incorrect stop word");
    }
}

    explicit SearchServer(const string& stop_words_text) 
        : SearchServer(SplitIntoWords(stop_words_text)) {
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
        if (document_id < 0) {
            throw invalid_argument("Document ID can't be negative");
        }
        if (documents_.count(document_id) > 0) {
            throw invalid_argument("Document ID already exists");
        }
        const auto words = SplitIntoWordsNoStop(document);
        if (!words) {
            throw invalid_argument("Document contains incorrect characters");
        }

        const double inv_word_count = 1.0 / words->size();
        for (const string& word : *words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
        document_ids_.push_back(document_id);
    }

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const {

        const auto query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, document_predicate);
        sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
            if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                return lhs.rating > rhs.rating;
            } else {
                return lhs.relevance > rhs.relevance;
            }
        });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }

    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const {
        return FindTopDocuments(raw_query, [status](int /*document_id*/, DocumentStatus document_status, int /*rating*/) {
            return document_status == status;
        });
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }

    int GetDocumentCount() const {
        return documents_.size();
    }

    int GetDocumentId(int index) const {
    return document_ids_.at(index);
    }

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
    const auto query = ParseQuery(raw_query); 
    vector<string> matched_words;
    
    for (const string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    
    return make_tuple(matched_words, documents_.at(document_id).status);
}

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    const set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;
    vector<int> document_ids_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    static bool IsValidWord(const string& word) {
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
        });
    }

    optional<vector<string>> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsValidWord(word)) {
                return nullopt;
            }
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    static int ComputeAverageRating(const vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = std::accumulate(ratings.begin(), ratings.end(), 0);
    return rating_sum / static_cast<int>(ratings.size());
    }

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    optional<QueryWord> ParseQueryWord(string text) const {
        if (text.empty()) {
        throw invalid_argument("Empty query word");
    }
    bool is_minus = false;

        if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }

        if (text.empty() || text[0] == '-' || !IsValidWord(text)) {
        throw invalid_argument("Incorrect query word: " + text);
    }

    return QueryWord{text, is_minus, IsStopWord(text)};
}

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
    Query result;
    for (const string& word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        result.plus_words.insert(query_word->data);
        if (query_word->is_minus) {
            result.minus_words.insert(query_word->data);
        }
    }
    return result; 
    }

    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto &[document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto &[document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto &[document_id, relevance] : document_to_relevance) {
            matched_documents.emplace_back(document_id, relevance, documents_.at(document_id).rating);
        }
        return matched_documents;
    }
};

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator first, Iterator second)
        : first_(first), second_(second) {}

    Iterator begin() const {
        return first_;
    }
    Iterator end() const {
        return second_;
    }
    size_t size() const {
        return distance(first_, second_);
    }

private:
    Iterator first_;
    Iterator second_;
};

template <typename Iterator>
ostream& operator<<(ostream& os, const IteratorRange<Iterator>& range) {
    for (auto it = range.begin(); it != range.end(); ++it) {
        os << *it;  
        if (next(it) != range.end()) {
            os << "";
        }
    }
    return os;
}

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator it_begin, Iterator it_end, size_t page_size) {
        for (auto it = it_begin; it != it_end;) {
            auto page = it;
            auto distance_to_end = distance(it, it_end);
            advance(page, min(page_size, static_cast<size_t>(distance_to_end)));
            page_range_.emplace_back(it, page);
            it = page;
        }
    }

    auto begin() const {
        return page_range_.begin();
    }

    auto end() const {
        return page_range_.end();
    }

private:
    vector<IteratorRange<Iterator>> page_range_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

template <typename SearchServerType>
class RequestQueue {
public:
    explicit RequestQueue(const SearchServerType& search_server)
        : search_server_(search_server), current_time_(0), no_result_request_(0) {}

    // "Обёртки" для всех методов поиска
    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
        return AddRequest(search_server_.FindTopDocuments(raw_query, document_predicate));
    }

    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status) {
        return AddRequest(search_server_.FindTopDocuments(raw_query, status));
    }

    vector<Document> AddFindRequest(const string& raw_query) {
        return AddRequest(search_server_.FindTopDocuments(raw_query));
    }

    int GetNoResultRequests() const {
        return no_result_request_;
    }

private:
    struct QueryResult {
        vector<Document> results;
        int timestamp;
    };

    deque<QueryResult> requests_;
    const SearchServerType& search_server_;
    int current_time_;
    int no_result_request_;
    const static int min_in_day_ = 1440;

    vector<Document> AddRequest(const vector<Document>& documents) {
        ++current_time_;
        while (!requests_.empty() && current_time_ - requests_.front().timestamp >= min_in_day_) {
            if (requests_.front().results.empty()) {
                --no_result_request_;
            }
            requests_.pop_front();
        }

        requests_.push_back({documents, current_time_});
        if (documents.empty()) {
            ++no_result_request_;
        }

        return documents;
    }
};

int main() {
    SearchServer search_server("and in at"s);
    RequestQueue<SearchServer> request_queue(search_server); // Указываем тип явно

    search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, {7, 2, 7});
    search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, {1, 2, 3});
    search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, {1, 2, 8});
    search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, {1, 3, 2});
    search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, {1, 1, 1});
    
    // 1439 запросов с нулевым результатом
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("empty request"s);
    }

    // ещё 1439 запросов с нулевым результатом
    request_queue.AddFindRequest("curly dog"s);

    // новые сутки, первый запрос удалён, 1438 запросов с нулевым результатом
    request_queue.AddFindRequest("big collar"s);

    // первый запрос удалён, 1437 запросов с нулевым результатом
    request_queue.AddFindRequest("sparrow"s);

    cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;

    return 0;
}