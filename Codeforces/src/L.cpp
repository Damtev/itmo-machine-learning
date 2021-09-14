//
// Created by damtev on 02.01.2021.
//

#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

double mean(const std::vector<int>& xs) {
  return ((double) std::accumulate(xs.begin(), xs.end(),0.0)) / xs.size();
}

double covariance(const std::vector<int>& x1s, const std::vector<int>& x2s, const double x1_mean, const double x2_mean) {
  const size_t n = x1s.size();
  double cov = 0;
  for (size_t i = 0; i < n; ++i) {
    cov += (x1s[i] - x1_mean) * (x2s[i] - x2_mean);
  }

  return cov;
}

double stddev(const std::vector<int>& xs, const double x_mean) {
  double dev = 0;
  for (const double x : xs) {
    dev += pow(x - x_mean, 2);
  }

  return sqrt(dev);
}

const double EPS = 1e-6;

int main() {
  size_t n;
  std::cin >> n;
  std::vector<int> x1s(n), x2s(n);
  for (size_t i = 0; i < n; ++i) {
    std::cin >> x1s[i] >> x2s[i];
  }

  const double x1_mean = mean(x1s);
  const double x2_mean = mean(x2s);

  const double denom = stddev(x1s, x1_mean) * stddev(x2s, x2_mean);
  const double pearson = (std::abs(denom) < EPS) ? 0 : covariance(x1s, x2s, x1_mean, x2_mean) / denom;
  std::cout << pearson;
}