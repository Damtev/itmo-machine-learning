//
// Created by damtev on 01.11.2020.
//

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

const double kEps = 1e-8;
const size_t kIterationsMultiplier = 10000;

struct Object {
  std::vector<double> kernel_values;
  int class_value;
};

class Dataset {
 public:
  int n = 0;
  double C = 0;
  std::vector<Object> objects;
};

double sign(const double value) {
  if (value < 0) {
    return -1;
  } else if (value > 0) {
    return 1;
  } else {
    return 0;
  }
}

size_t GetUniformDistribution(const size_t n, const size_t index, std::mt19937& mersenne) {
  const size_t value = (std::uniform_int_distribution<size_t>(0, n - 2))(mersenne);

  return value < index ? value : value + 1;
}

double GetE(const Dataset& dataset, const std::vector<double>& alpha, const size_t index) {
  double sum = 0;
  for (size_t i = 0; i < dataset.n; ++i) {
    sum += dataset.objects[i].class_value * alpha[i] * dataset.objects[i].kernel_values[index];
  }

  return sum - dataset.objects[index].class_value;
}

std::pair<double, double> Get_L_H(const Dataset& dataset,
                                  const std::vector<double>& alpha,
                                  const size_t i,
                                  const size_t j) {
  if (dataset.objects[i].class_value != dataset.objects[j].class_value) {
    return {std::max(0.0, alpha[j] - alpha[i]), std::min(dataset.C, dataset.C + alpha[j] - alpha[i])};
  } else {
    return {std::max(0.0, alpha[i] + alpha[j] - dataset.C), std::min(dataset.C, alpha[i] + alpha[j])};
  }
}

double GetShift(const Dataset& dataset, const std::vector<double>& alpha, const size_t index) {
  double result = 0;
  for (size_t i = 0; i < dataset.n; ++i) {
    result += alpha[i] * dataset.objects[i].class_value * dataset.objects[i].kernel_values[index];
  }

  return 1.0 / dataset.objects[index].class_value - result;
}

double CalculateShift(const Dataset& dataset, const std::vector<double>& alpha) {
  for (size_t i = 0; i < dataset.n; ++i) {
    if (alpha[i] > kEps && alpha[i] + kEps > dataset.C) {
      return GetShift(dataset, alpha, i);
    }
  }

  double b = 0;
  size_t count = 0;
  for (size_t i = 0; i < dataset.n; ++i) {
    if (alpha[i] > kEps) {
      b += GetShift(dataset, alpha, i);
      ++count;
    }
  }

  return b / count;
}

double Fit(Dataset& dataset,
           std::vector<double>& alpha) {
  const size_t n = dataset.n;
  using namespace std;
  random_device rd;
  mt19937 mersenne(rd());

  vector<size_t> indices(n);
  iota(indices.begin(), indices.end(), 0);

  const size_t iterations = n * kIterationsMultiplier;
  size_t iteration = 0;
  while (iteration < iterations) {
    shuffle(indices.begin(), indices.end(), mersenne);
    for (size_t index_index = 0; index_index < n && iteration < iterations; ++index_index, ++iteration) {
      const size_t i = indices[index_index];
      const size_t j = indices[GetUniformDistribution(n, index_index, mersenne)];

      const auto [L, H] = Get_L_H(dataset, alpha, i, j);
      if (abs(L - H) < kEps) {
        continue;
      }

      const double eta = 2 * dataset.objects[i].kernel_values[j] - dataset.objects[i].kernel_values[i]
          - dataset.objects[j].kernel_values[j];
      if (eta > -kEps) {
        continue;
      }

      const double alpha2 = alpha[j];
      const double E_i = GetE(dataset, alpha, i);
      const double E_j = GetE(dataset, alpha, j);

      double a2 = alpha2 + dataset.objects[j].class_value * (E_j - E_i) / eta;
      a2 = min(max(L, a2), H);
      if (abs(a2 - alpha2) < kEps * (a2 + alpha2 + kEps)) {
        continue;
      }

      alpha[j] = a2;
      alpha[i] += dataset.objects[i].class_value * dataset.objects[j].class_value * (alpha2 - a2);
    }
  }

  return CalculateShift(dataset, alpha);
}

int main() {
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::ios_base::sync_with_stdio(false);

  Dataset dataset;
  std::cin >> dataset.n;
  dataset.objects.resize(dataset.n);
  for (size_t i = 0; i < dataset.n; ++i) {
    for (size_t j = 0; j < dataset.n; j++) {
      int kernel_value;
      std::cin >> kernel_value;
      dataset.objects[i].kernel_values.push_back(kernel_value);
    }
    int class_value;
    std::cin >> class_value;
    dataset.objects[i].class_value = class_value;
  }
  std::cin >> dataset.C;

  std::vector<double> alpha(dataset.n);
  const double b = Fit(dataset, alpha);

  std::cout << std::fixed;
  std::cout.precision(12);
  for (const double cf : alpha) {
    std::cout << cf << "\n";
  }
  std::cout << b;

  return 0;
}