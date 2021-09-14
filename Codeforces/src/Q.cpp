//
// Created by damtev on 02.01.2021.
//

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>

int main() {
  size_t kx, ky, n;
  std::cin >> kx >> ky >> n;
  std::unordered_map<int, double> px;
  std::unordered_map<int, std::unordered_map<int, double>> pxy;
  for (size_t i = 0; i < n; ++i) {
    int x, y;
    std::cin >> x >> y;
    px[x] += 1.0 / n;
    pxy[x][y] += 1.0 / n;
  }

  double entropy = 0;
  for (const auto& [x, p] : pxy) {
    for (const auto& [_, cur_p] : p) {
      entropy += -cur_p * (log(cur_p) - log(px[x]));
    }
  }

  std::cout << std::fixed << entropy;
}