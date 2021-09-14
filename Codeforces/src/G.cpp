//
// Created by damtev on 10.11.2020.
//

#include <iostream>
#include <vector>
#include <variant>
#include <memory>
#include <algorithm>
#include <optional>
#include <cmath>

#define forn(i, n) for(long long i = 0; i < n; ++i)

const size_t kMaxEntropySize = 200;

struct Object {
  std::vector<long long> signs;
  long long klass{};

  explicit Object(const long long m) {
    signs.resize(m);
  }

  long long& operator[](const long long i) {
    return signs[i];
  }

  long long operator[](const long long i) const {
    return signs[i];
  }

  bool operator<(const Object& object) const {
    return klass < object.klass;
  }

  [[nodiscard]] long long size() const {
    return signs.size();
  }
};

struct Dataset {
  const long long n, m;
  std::vector<Object> objects;

  Dataset(const long long n, const long long m) : n(n), m(m) {
    objects.resize(n, Object(m));
  }

  Object& operator()(const long long i) {
    return objects[i];
  }

  long long& operator()(const long long i, const long long j) {
    return (operator()(i))[j];
  }

  [[nodiscard]] auto begin() {
    return objects.begin();
  }

  [[nodiscard]] auto end() {
    return objects.end();
  }
};

struct Node;

struct TreeNode {
  long long feature_index{};
  long double limit{};
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
};

struct LeafNode {
  long long group;
};

struct Node {
  long long id{};
  std::variant<LeafNode, TreeNode> data;

  friend std::ostream& operator<<(std::ostream& os, const Node& node) {
    if (std::holds_alternative<LeafNode>(node.data)) {
      os << "C " << std::get<LeafNode>(node.data).group << std::endl;
    } else {
      const auto& cur_node = std::get<TreeNode>(node.data);
      os << "Q " << cur_node.feature_index + 1 << " " << cur_node.limit << " " << cur_node.left->id << " "
         << cur_node.right->id << std::endl;
      os << *(cur_node.left);
      os << *(cur_node.right);
    }

    return os;
  }
};

long long FindMostFrequentElement(const long long classes,
                                  std::vector<Object>::const_iterator begin,
                                  std::vector<Object>::const_iterator end) {
  std::vector<long long> classes_count(classes + 1, 0);
  for (auto it = begin; it != end; ++it) {
    ++classes_count[it->klass];
  }

  return std::max_element(classes_count.begin() + 1, classes_count.end()) - classes_count.begin();
}

void Change(long long& value, long long& sum, const long long delta) {
  sum -= value * value;
  value += delta;
  sum += value * value;
}

class DecisionTree {
  long long cur_id = 0;
  const long long classes;
  std::unique_ptr<Node> root = std::make_unique<Node>();
  const long long max_depth;

  using splitter_function_t = std::optional<std::pair<long long, long double>>(DecisionTree::*)(const long long,
                                                                                                std::vector<Object>::iterator,
                                                                                                std::vector<Object>::iterator) const;

  splitter_function_t splitter_function{};

 public:
  explicit DecisionTree(const long long max_depth, const long long classes) : max_depth(max_depth), classes(classes) {}

  void Fit(Dataset& dataset) {
    splitter_function =
        dataset.objects.size() < kMaxEntropySize ? &DecisionTree::SplitEntropy : &DecisionTree::SplitGini;
    FitNode(*root, 0, dataset.begin(), dataset.end());
  }

  void FitNode(Node& node,
               const long long depth,
               std::vector<Object>::iterator begin,
               std::vector<Object>::iterator end) {
    node.id = ++cur_id;
    const long long min_klass = std::min_element(begin, end)->klass;
    const long long max_klass = std::max_element(begin, end)->klass;
    if (min_klass == max_klass) {
      node.data = LeafNode{min_klass};
      return;
    }

    if (depth == max_depth) {
      node.data = LeafNode{FindMostFrequentElement(classes, begin, end)};
      return;
    }

    std::optional<std::pair<long long, long double>>
        best_splitter = (this->*splitter_function)(begin->size(), begin, end);
    if (best_splitter.has_value()) {
      TreeNode new_node;
      new_node.feature_index = best_splitter->first;
      new_node.limit = best_splitter->second;
      new_node.left = std::make_unique<Node>();
      new_node.right = std::make_unique<Node>();
      auto mid = std::partition(begin, end, [best_splitter](const Object& object) {
        return object[best_splitter->first] < best_splitter->second;
      });
      FitNode(*new_node.left, depth + 1, begin, mid);
      FitNode(*new_node.right, depth + 1, mid, end);
      node.data = std::move(new_node);
    } else {
      node.data = LeafNode{FindMostFrequentElement(classes, begin, end)};
    }
  }

  static long double CalculateEntropy(const std::vector<long long>& counts, const long long size) {
    long double sum = 0;
    for (const long long count: counts) {
      if (count != 0) {
        sum -= ((long double) count / (long double) size) * log((double) count / (double) size);
      }
    }

    return sum;
  }

  [[nodiscard]] std::optional<std::pair<long long, long double>> SplitGini(const long long features_cnt,
                                                                           std::vector<Object>::iterator begin,
                                                                           std::vector<Object>::iterator end) const {
    std::optional<long double> best_score;
    std::optional<std::pair<long long, long double>> best_splitter;
    forn(i, features_cnt) {
      std::sort(begin, end, [i](const Object& a, const Object& b) {
        return a[i] < b[i];
      });
      if (begin->operator[](i) == (end - 1)->operator[](i)) {
        continue;
      }

      std::vector<long long> left_count(classes + 1);
      std::vector<long long> right_count(classes + 1);
      long long left_size = 0;
      long long right_size = distance(begin, end);
      long long left_sum = 0;
      long long right_sum = 0;

      for (auto it = begin; it != end; ++it) {
        Change(right_count[it->klass], right_sum, 1);
      }

      long long prev = -1;
      for (auto mid = begin; mid != end; ++mid) {
        if (mid != begin && mid->operator[](i) != prev) {
          const long double score = (long double) left_sum / left_size + (long double) right_sum / right_size;
          if (score > best_score) {
            best_splitter = std::pair<long long, long double>{i, (long double) (prev + mid->operator[](i)) / 2.0};
            best_score = score;
          }
        }
        Change(right_count[mid->klass], right_sum, -1);
        Change(left_count[mid->klass], left_sum, 1);
        ++left_size;
        --right_size;
        prev = mid->operator[](i);
      }
    }

    return best_splitter;
  }

  [[nodiscard]] std::optional<std::pair<long long, long double>> SplitEntropy(const long long features_cnt,
                                                                              std::vector<Object>::iterator begin,
                                                                              std::vector<Object>::iterator end) const {
    std::optional<long double> best_score;
    std::optional<std::pair<long long, long double>> best_splitter;
    forn(i, features_cnt) {
      std::sort(begin, end, [i](const Object& a, const Object& b) {
        return a[i] < b[i];
      });
      if (begin->operator[](i) == (end - 1)->operator[](i)) {
        continue;
      }

      std::vector<long long> left_count(classes + 1);
      std::vector<long long> right_count(classes + 1);
      long long left_size = 0;
      long long right_size = distance(begin, end);

      for (auto it = begin; it != end; ++it) {
        ++right_count[it->klass];
      }

      long long prev = -1;
      for (auto mid = begin; mid != end; ++mid) {
        if (mid != begin && mid->operator[](i) != prev) {
          const long double score = CalculateEntropy(left_count, left_size) * left_size
              + CalculateEntropy(right_count, right_size) * right_size;
          if (!best_score.has_value() || (score < *best_score)) {
            best_splitter = std::pair<long long, long double>{i, (long double) (prev + mid->operator[](i)) / 2.0};
            best_score = score;
          }
        }
        --right_count[mid->klass];
        ++left_count[mid->klass] += 1;
        ++left_size;
        --right_size;
        prev = mid->operator[](i);
      }
    }

    return best_splitter;
  }

  friend std::ostream& operator<<(std::ostream& os, const DecisionTree& decision_tree) {
    os << decision_tree.cur_id << std::endl;
    os << *decision_tree.root;

    return os;
  }
};

int main() {
  long long m, k, h;
  std::cin >> m >> k >> h;

  long long n;
  std::cin >> n;
  Dataset dataset(n, m);
  forn(i, n) {
    forn(j, m) {
      std::cin >> dataset(i)[j];
    }
    std::cin >> dataset(i).klass;
  }

  DecisionTree decision_tree(h, k);
  decision_tree.Fit(dataset);
  std::cout << std::fixed;
  std::cout.precision(12);
  std::cout << decision_tree;
  return 0;
}