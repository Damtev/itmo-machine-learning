//
// Created by damtev on 02.01.2021.
//

#include <iostream>
#include <vector>
#include <unordered_map>

int main() {
  size_t k1, k2, n;
  std::cin >> k1 >> k2 >> n;
  std::unordered_map<int, double> f1, f2;
  std::unordered_map<int, std::unordered_map<int, int>> table;
  for (size_t i = 0; i < n; ++i) {
    int x1, x2;
    std::cin >> x1 >> x2;
    f1[x1] += 1.0 / n;
    f2[x2] += 1.0 / n;
    ++table[x1][x2];
  }

  double pearson = n;
  for (const auto& [x1, p] : table) {
    for (const auto& [x2, real_value] : p) {
      const double expected_value = f1[x1] * f2[x2] * n;
      const double diff = real_value - expected_value;
      pearson = pearson - expected_value + (diff * diff)/expected_value;
    }
  }

  std::cout << std::fixed << pearson;
}