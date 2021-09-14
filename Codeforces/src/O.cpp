//
// Created by damtev on 02.01.2021.
//

#include <iostream>
#include <vector>
#include <unordered_map>

int main() {
  size_t k, n;
  std::cin >> k >> n;
  std::vector<int> xs(n), ys(n);
  for (size_t i = 0; i < n; ++i) {
    std::cin >> xs[i] >> ys[i];
  }

  long long sum = 0;
  for (const long long y : ys) {
    sum += y * y;
  }
  const double ee = static_cast<double>(sum) / static_cast<double>(n);

  std::unordered_map<int, std::pair<double, double>> conditional_y_from_x;
  for (size_t i = 0; i < n; ++i) {
    auto& [e, count] = conditional_y_from_x[xs[i]];
    const int y = ys[i];
    e += y / static_cast<double>(n);
    count += 1.0 / static_cast<double>(n);
  }

  double square_conditional = 0;
  for (const auto& p : conditional_y_from_x) {
    const auto& [e, count] = p.second;
    square_conditional += static_cast<double>(e) * e / count;
  }

  const double variance_expectation = ee - square_conditional;

  std::cout << variance_expectation;
}