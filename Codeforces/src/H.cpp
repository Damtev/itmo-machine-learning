//
// Created by damtev on 02.01.2021.
//

#include <iostream>
#include <vector>
#include <cmath>

int main() {
  size_t m;
  std::cin >> m;
  std::vector<size_t> neurons{static_cast<size_t>(std::pow(m, 2)), 1};
  std::vector<double> values(neurons.front());
  for (size_t i = 0; i < neurons.front(); ++i) {
    std::cin >> values[i];
  }

  const size_t d = 2;
  std::cout << d << std::endl;

  std::cout << neurons.front() << " " << neurons.back() << std::endl;

  for (size_t i = 0; i < neurons.front(); ++i) {
    for (size_t position = 0; position < m; ++position) {
      double cur_value;
      if ((i & (static_cast<size_t>(std::pow(position, 2)))) == 0) {
        cur_value = -1000;
      } else {
        cur_value = 1;
      }
      std::cout << cur_value << " ";
    }

    size_t copy = i;
    size_t ones = 0;
    while (copy > 0) {
      ++ones;
      copy &= (copy - 1);
    }

    std::cout << 0.5 - ones << std::endl;
  }

  values.push_back(-0.5);
  for (const double value : values) {
    std::cout << value << " ";
  }

  return 0;
}