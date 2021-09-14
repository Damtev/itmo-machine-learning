//
// Created by damtev on 08.01.2021.
//

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

/// from https://habr.com/ru/post/166201/
#define SWITCH(str)  switch(s_s::str_hash_for_switch(str))
#define CASE(str)    static_assert(s_s::str_is_correct(str) && (s_s::str_len(str) <= s_s::MAX_LEN),\
"CASE string contains wrong characters, or its length is greater than 9");\
case s_s::str_hash(str, s_s::str_len(str))
#define DEFAULT  default

namespace s_s
{
  typedef unsigned char uchar;
  typedef unsigned long long ullong;

  const uchar MAX_LEN = 9;
  const ullong N_HASH = static_cast<ullong>(-1);

  constexpr ullong raise_128_to(const uchar power)
  {
    return 1ULL << 7 * power;
  }

  constexpr bool str_is_correct(const char* const str)
  {
    return (static_cast<signed char>(*str) > 0) ? str_is_correct(str + 1) : (*str ? false : true);
  }

  constexpr uchar str_len(const char* const str)
  {
    return *str ? (1 + str_len(str + 1)) : 0;
  }

  constexpr ullong str_hash(const char* const str, const uchar current_len)
  {
    return *str ? (raise_128_to(current_len - 1) * static_cast<uchar>(*str) + str_hash(str + 1, current_len - 1)) : 0;
  }

  inline ullong str_hash_for_switch(const char* const str)
  {
    return (str_is_correct(str) && (str_len(str) <= MAX_LEN)) ? str_hash(str, str_len(str)) : N_HASH;
  }

  inline ullong str_hash_for_switch(const std::string& str)
  {
    return (str_is_correct(str.c_str()) && (str.length() <= MAX_LEN)) ? str_hash(str.c_str(), str.length()) : N_HASH;
  }
}

using Line = std::vector<double>;
using Matrix = std::vector<Line>;

struct Node {
  Matrix value;
  Matrix derivative;
  
  virtual void Forward() = 0;

  virtual void Backward() = 0;

  void InitDifference() {
    derivative.resize(value.size(), Line(value.front().size()));
  }
};

using NodePtr = std::unique_ptr<Node>;

struct Var : Node {
  Var(const size_t rows, const size_t columns) {
    value = Matrix(rows, Line(columns));
  }

  void Forward() override {
    InitDifference();
  }

  void Backward() override {}
};

struct Tnh : Node {
  NodePtr source;

  explicit Tnh(Node* source) : source(source) {}

  void Forward() override {
    value = source->value;

    for (Line& line : value) {
      for (double& element : line) {
        element = tanh(element);
      }
    }

    InitDifference();
  }

  void Backward() override {
    for (size_t i = 0; i < derivative.size(); ++i) {
      for (size_t j = 0; j < derivative.front().size(); ++j) {
        source->derivative[i][j] += derivative[i][j] * (1.0 - pow(value[i][j], 2));
      }
    }
  }
};

struct Rlu : Node {
  NodePtr source;
  double inverted_alpha;

  Rlu(Node* source, double inverted_alpha) : source(source), inverted_alpha(inverted_alpha) {}

  void Forward() override {
    value = source->value;

    for (Line& line : value) {
      for (double& element : line) {
        if (element < 0) {
          element /= inverted_alpha;
        }
      }
    }

    InitDifference();
  }

  void Backward() override {
    for (size_t i = 0; i < derivative.size(); ++i) {
      for (size_t j = 0; j < derivative.front().size(); ++j) {
        double cur_value = derivative[i][j];
        if (source->value[i][j] < 0) {
          cur_value /= inverted_alpha;
        }
        source->derivative[i][j] += cur_value;
      }
    }
  }
};

struct Mul : Node {
  NodePtr left;
  NodePtr right;

  Mul(Node* left, Node* right) : left(left), right(right) {}

  void Forward() override {
    value = Matrix(left->value.size(), Line(right->value.front().size(), 0));
    for (size_t i = 0; i < value.size(); ++i) {
      for (size_t j = 0; j < value.front().size(); ++j) {
        for (size_t l = 0; l < right->value.size(); ++l) {
          value[i][j] += left->value[i][l] * right->value[l][j];
        }
      }
    }

    InitDifference();
  }

  void Backward() override {
    const size_t n = left->value.size();
    const size_t m = right->value.size();
    const size_t k = right->value.front().size();

    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < m; ++j) {
        for (size_t l = 0; l < k; ++l) {
          left->derivative[i][j] += right->value[j][l] * derivative[i][l];
        }
      }
    }

    for (size_t i = 0; i < m; ++i) {
      for (size_t j = 0; j < k; ++j) {
        for (size_t l = 0; l < n; ++l) {
          right->derivative[i][j] += left->value[l][i] * derivative[l][j];
        }
      }
    }
  }
};

struct Sum : Node {
  std::vector<Node*> nodes;

  explicit Sum(std::vector<Node*>  nodes) : nodes(std::move(nodes)) {}

  void Forward() override {
    value = nodes.front()->value;

    for (size_t i = 1; i < nodes.size(); ++i) {
      for (size_t j = 0; j < value.size(); ++j) {
        for (size_t l = 0; l < value.front().size(); ++l) {
          value[j][l] += nodes[i]->value[j][l];
        }
      }
    }

    InitDifference();
  }

  void Backward() override {
    for (Node* node : nodes) {
      for (size_t j = 0; j < derivative.size(); ++j) {
        for (size_t l = 0; l < value.front().size(); ++l) {
          node->derivative[j][l] += derivative[j][l];
        }
      }
    }
  }
};

struct Had : Node {
  std::vector<Node*> nodes;

  explicit Had(std::vector<Node*>  nodes) : nodes(std::move(nodes)) {}

  void Forward() override {
    value = nodes.front()->value;

    for (size_t i = 1; i < nodes.size(); ++i) {
      for (size_t j = 0; j < value.size(); ++j) {
        for (size_t l = 0; l < value.front().size(); ++l) {
          value[j][l] *= nodes[i]->value[j][l];
        }
      }
    }

    InitDifference();
  }

  void Backward() override {
    for (size_t i = 0; i < nodes.size(); ++i) {
      Matrix derivative_copy = derivative;
      for (size_t j = 0; j < nodes.size(); ++j) {
        if (i == j) {
          continue;
        }

        for (size_t k = 0; k < value.size(); ++k) {
          for (size_t l = 0; l < value.front().size(); ++l) {
            derivative_copy[k][l] *= nodes[j]->value[k][l];
          }
        }
      }

      for (size_t k = 0; k < derivative_copy.size(); ++k) {
        for (size_t l = 0; l < derivative_copy.front().size(); ++l) {
          nodes[i]->derivative[k][l] += derivative_copy[k][l];
        }
      }
    }
  }
};

class Network {
  size_t n, m, k;

  void ReadValues() {
    for (size_t i = 0; i < m; ++i) {
      Node* node = nodes[i];
      for (Line& line : node->value) {
        for (double& element : line) {
          std::cin >> element;
        }
      }
    }
  }

  void Forward() {
    for (size_t i = 0; i < n; ++i) {
      Node* node = nodes[i];
      node->Forward();
    }
  }

  void PrintValues() {
    for (size_t i = 0; i < k; ++i) {
      Node* node = nodes[nodes.size() - k + i];
      for (const Line& line : node->value) {
        for (const double& element : line) {
          std::cout << element << " ";
        }

        std::cout << "\n";
      }
    }
  }

  void ReadDerivatives() {
    for (size_t i = 0; i < k; ++i) {
      Node* node = nodes[nodes.size() - k + i];
      for (Line& line : node->derivative) {
        for (double& element : line) {
          std::cin >> element;
        }
      }
    }
  }

  void Backward() {
    for (auto iter = nodes.rbegin(); iter != nodes.rend(); ++iter) {
      (*iter)->Backward();
    }
  }

  void PrintDerivatives() {
    for (size_t i = 0; i < m; ++i) {
      Node* node = nodes[i];
      for (Line& line : node->derivative) {
        for (double& element : line) {
          std::cout << element << " ";
        }

        std::cout << "\n";
      }
    }
  }

 public:
  std::vector<Node*> nodes;

  Network(size_t n, size_t m, size_t k) : n(n), m(m), k(k) {}

  void Pipeline() {
    ReadValues();
    Forward();
    PrintValues();
    ReadDerivatives();
    Backward();
    PrintDerivatives();
  }
};

int main() {
  size_t n, m, k;
  std::cin >> n >> m >> k;
  Network network(n, m, k);
  for (size_t i = 0; i < n; ++i) {
    std::string command;
    std::cin >> command;
    SWITCH(command) {
      CASE("var"):
        size_t r, c;
        std::cin >> r >> c;
        network.nodes.push_back(new Var(r, c));
        break;
      CASE("tnh"):
      {
        size_t x;
        std::cin >> x;
        network.nodes.push_back(new Tnh(network.nodes[x - 1]));
      }
        break;
      CASE("rlu"):
        size_t inverted_alpha, x;
        std::cin >> inverted_alpha >> x;
        network.nodes.push_back(new Rlu(network.nodes[x - 1], inverted_alpha));
        break;
      CASE("mul"):
        size_t a, b;
        std::cin >> a >> b;
        network.nodes.push_back(new Mul(network.nodes[a - 1], network.nodes[b - 1]));
        break;
      CASE("sum"):
      CASE("had"):
      {
        size_t len;
        std::cin >> len;
        std::vector<Node*> args(len);
        for (size_t j = 0; j < len; ++j) {
          size_t index;
          std::cin >> index;
          args[j] = network.nodes[index - 1];
        }

        Node* new_node;
        if (command == "sum") {
          new_node = new Sum(args);
        } else {
          new_node = new Had(args);
        }
        network.nodes.push_back(new_node);
      }
        break;
      DEFAULT:
        return 1;
    }
  }

  network.Pipeline();

  return 0;
}