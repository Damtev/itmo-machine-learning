//
// Created by damtev on 02.01.2021.
//

#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>

int main() {
  size_t n;
  std::cin >> n;
  std::vector<long long> xs(n), ys(n);
  for (size_t i = 0; i < n; ++i) {
    std::cin >> xs[i] >> ys[i];
  }

  if (n == 1) {
    std::cout << 0.0;
    return 0;
  }

  std::set<long long> sorted_x(xs.begin(), xs.end());
  std::set<long long> sorted_y(ys.begin(), ys.end());

  std::unordered_map<long long, long long> x_rank, y_rank;
  long long rank = 1;
  for (const long long x : sorted_x) {
    x_rank[x] = rank++;
  }
  rank = 1;
  for (const long long y : sorted_y) {
    y_rank[y] = rank++;
  }

  double spearman = 0;
  for (size_t i = 0; i < n; ++i) {
    const long long x = xs[i];
    const long long y = ys[i];
    long long cur_diff = (x_rank[x] - y_rank[y]) * (x_rank[x] - y_rank[y]);
    spearman += (double) cur_diff;
  }

  spearman = 1.0 - 6.0 * spearman / ((n - 1.0) * n * (n + 1.0));
  std::cout << spearman;
}