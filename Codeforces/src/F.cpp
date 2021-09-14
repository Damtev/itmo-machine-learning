//
// Created by damtev on 08.11.2020.
//

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

int main() {
  using namespace std;

  size_t k, n, m;
  cin >> k;

  vector<size_t> lambdas(k), class_count(k);
  for (size_t c = 0; c < k; ++c) {
    cin >> lambdas[c];
  }

  size_t alpha;
  cin >> alpha;

  cin >> n;
  vector<unordered_map<string, size_t>> count_words_in_classes(k);
  std::unordered_set<string> all_words;
  for (size_t i = 0; i < n; ++i) {
    size_t c;
    cin >> c;
    --c;
    ++class_count[c];

    size_t words_count;
    cin >> words_count;

    unordered_set<string> message_words;
    for (size_t j = 0; j < words_count; ++j) {
      string word;
      cin >> word;
      message_words.insert(word);
    }

    all_words.insert(message_words.begin(), message_words.end());
    for (const string& word : message_words) {
      ++count_words_in_classes[c][word];
    }
  }

  vector<unordered_map<string, long double>> words_probabilities(k);
  for (size_t c = 0; c < k; ++c) {
    const auto words_probability_denominator = (long double) (class_count[c] + 2 * alpha);

    for (const string& word : all_words) {
      words_probabilities[c][word] = (count_words_in_classes[c][word] + alpha) / words_probability_denominator;
    }
  }

  vector<long double> classes_prior_probabilities(k);
  for (size_t c = 0; c < k; ++c) {
    classes_prior_probabilities[c] = ((long double) class_count[c]) / n;
  }

  cin >> m;
  for (size_t i = 0; i < m; ++i) {
    size_t words_count;
    cin >> words_count;

    unordered_set<string> message_words;
    for (size_t j = 0; j < words_count; ++j) {
      string word;
      cin >> word;
      message_words.insert(word);
    }

    vector<long double> answer(k);
    long double denominator = 0;
    for (size_t c = 0; c < k; ++c) {
      answer[c] = lambdas[c] * classes_prior_probabilities[c];
      for (const string& word : all_words) {
        if (message_words.count(word) > 0) {
          answer[c] *= words_probabilities[c][word];
        } else {
          answer[c] *= (1 - words_probabilities[c][word]);
        }
      }
      denominator += answer[c];
    }

    for (size_t c = 0; c < k; ++c) {
      answer[c] /= denominator;
      cout << answer[c] << " ";
    }

    cout << endl;
  }

  return 0;
}