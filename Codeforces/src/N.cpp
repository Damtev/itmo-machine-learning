//
// Created by damtev on 02.01.2021.
//

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

long long sum_differences(const std::vector<int>& xs) {
  const long long n = xs.size();
  long long sum = 0;
  for (long long i = 0; i < n; ++i) {
    sum += (2 * i - n + 1) * xs[i];
  }

  return sum;
}

long long inner_distance(const std::vector<int>& xs,
                         const std::unordered_map<long long, std::vector<int>>& classes_indices) {
  long long sum = 0;
  for (const auto& p : classes_indices) {
    const std::vector<int>& indices = p.second;
    std::vector<int> cur_xs;
    const long long n = indices.size();
    cur_xs.reserve(n);
    for (const int index : indices) {
      cur_xs.push_back(xs[index]);
    }

    std::sort(cur_xs.begin(), cur_xs.end());
    sum += sum_differences(cur_xs);
  }

  return sum * 2LL;
}

int main() {
  size_t k, n;
  std::cin >> k >> n;
  std::vector<int> xs(n);
  std::unordered_map<long long, std::vector<int>> classes_indices;
  for (size_t i = 0; i < n; ++i) {
    std::cin >> xs[i];
    int y;
    std::cin >> y;
    classes_indices[y].push_back(i);
  }

  const long long inner = inner_distance(xs, classes_indices);
  std::sort(xs.begin(), xs.end());
  const long long all = sum_differences(xs) * 2LL;
  const long long outer = all - inner;
  std::cout << inner << std::endl << outer;
}