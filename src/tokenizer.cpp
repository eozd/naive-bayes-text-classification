#include "tokenizer.hpp"
#include "util.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <file_manager.hpp>
#include <fstream>
#include <iostream>
#include <numeric>

// tell the compiler that stem will be externally linked
extern int stem(char* p, int i, int j);

ir::Tokenizer::Stats::Stats()
    : total_unnormalized_tokens(0), total_normalized_tokens(0),
      total_unnormalized_terms(0), total_normalized_terms(0) {}

std::vector<std::pair<std::string, size_t>>
ir::Tokenizer::tokenize(const std::string& str) {
    std::string str_copy(str);
    auto tokens = split(str_copy, " \t\n\r\v\f");

    std::vector<std::pair<std::string, size_t>> result;
    for (size_t i = 0; i < tokens.size(); ++i) {
        result.emplace_back(tokens[i], i);
    }
    m_stats.total_unnormalized_tokens += result.size();

    return result;
}

std::string ir::Tokenizer::remove_punctuation(const std::string& token) {
    std::string result(token);

    // remove certain puncts from anywhere in the word
    std::replace(result.begin(), result.end(), '\"', '\'');
    std::replace(result.begin(), result.end(), ',', '\'');
    std::replace(result.begin(), result.end(), '<', '\'');
    std::replace(result.begin(), result.end(), '>', '\'');
    result.erase(std::remove(result.begin(), result.end(), '\''), result.end());

    if (result.empty()) {
        return result;
    }

    // iterate as long as current char is not alphanumeric and not '
    // (' will be removed)
    auto to_remove = [](const char c) { return c != '\'' && !isalnum(c); };
    // remove any kind of punct from the start and end of the word
    for (size_t i = 0; to_remove(result[i]); ++i) {
        result[i] = '\'';
    }
    for (size_t i = result.size() - 1; to_remove(result[i]); --i) {
        result[i] = '\'';
    }
    result.erase(std::remove(result.begin(), result.end(), '\''), result.end());

    return result;
}

bool ir::Tokenizer::is_stopword(const std::string& word) {
    static std::vector<std::string> stopwords;

    // if calling for the first time
    if (stopwords.empty()) {
        std::ifstream ifs(ir::STOPWORD_PATH);
        std::string stopword;
        while (ifs >> stopword) {
            stopwords.push_back(stopword);
        }
        assert(!stopwords.empty());

        std::sort(stopwords.begin(), stopwords.end());
    }

    return std::binary_search(stopwords.begin(), stopwords.end(), word);
}

std::string ir::Tokenizer::normalize(const std::string& token) {
    // remove punctuation using heuristics
    std::string result = remove_punctuation(token);
    // convert string to lowercase
    std::transform(result.begin(), result.end(), result.begin(), tolower);
    // if string is a stopword, return empty string
    if (is_stopword(result)) {
        return "";
    }
    // stem the word
    auto word_end = static_cast<size_t>(stem(&result[0], 0, result.size() - 1));
    result = result.substr(0, word_end + 1);

    return result;
}

void ir::Tokenizer::normalize_all(std::vector<std::string>& token_vec) {
    // normalize all words in-place
    std::transform(
        token_vec.begin(), token_vec.end(), token_vec.begin(),
        [this](const std::string& token) { return this->normalize(token); });
    // remove empty strings
    token_vec.erase(std::remove(token_vec.begin(), token_vec.end(), ""),
                    token_vec.end());
}

std::vector<std::pair<std::string, size_t>>
ir::Tokenizer::get_doc_terms(const raw_doc& doc) {
    auto tokens_indices = tokenize(doc);

    std::for_each(
        tokens_indices.begin(), tokens_indices.end(),
        [this](const auto& pair) { ++(this->unnormalized_terms[pair.first]); });

    std::transform(
        tokens_indices.begin(), tokens_indices.end(), tokens_indices.begin(),
        [this](const auto& token_pair) -> std::pair<std::string, size_t> {
            return {this->normalize(token_pair.first), token_pair.second};
        });

    tokens_indices.erase(std::remove_if(tokens_indices.begin(),
                                        tokens_indices.end(),
                                        [](const auto& token_pair) {
                                            return token_pair.first.empty();
                                        }),
                         tokens_indices.end());

    std::for_each(
        tokens_indices.begin(), tokens_indices.end(),
        [this](const auto& pair) { ++(this->normalized_terms[pair.first]); });

    m_stats.total_unnormalized_terms = unnormalized_terms.size();
    m_stats.total_normalized_terms = normalized_terms.size();
    m_stats.total_normalized_tokens += tokens_indices.size();

    return tokens_indices;
}

ir::Tokenizer::Stats ir::Tokenizer::stats() {
    std::vector<std::pair<std::string, size_t>> unnormalized(
        unnormalized_terms.begin(), unnormalized_terms.end());
    std::vector<std::pair<std::string, size_t>> normalized(
        normalized_terms.begin(), normalized_terms.end());

    std::sort(unnormalized.begin(), unnormalized.end(),
              [](const auto& left, const auto& right) {
                  return left.second > right.second;
              });
    std::sort(normalized.begin(), normalized.end(),
              [](const auto& left, const auto& right) {
                  return left.second > right.second;
              });

    for (size_t i = 0; i < Stats::TopTermCount; ++i) {
        m_stats.top_unnormalized_terms[i] = unnormalized[i].first;
        m_stats.top_normalized_terms[i] = normalized[i].first;
    }

    return m_stats;
}
