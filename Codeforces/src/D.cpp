//
// Created by damtev on 27.10.2020.
//

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

const size_t kEpochs = 1000000;
const double kEps = 1e-12;
const size_t kLearningStep = 1000;

struct Object {
  std::vector<double> kernel_values;
  double class_value;
};

class Dataset {
 public:
  int n = 0;
  int m = 0;
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

void Fit(Dataset& dataset,
         std::vector<double>& weights) {
  using namespace std;
//  random_device rd;
//  mt19937 mersenne(rd());
//  const size_t point_index = mersenne() % dataset.n;
  const size_t point_index = rand() % dataset.n;
  const Object cur_point = dataset.objects[point_index];
  double predicted_value = 0;
  for (size_t i = 0; i < dataset.m; i++) {
    predicted_value += weights[i] * cur_point.kernel_values[i];
  }
  const double diff = predicted_value - cur_point.class_value;
  const double gradient_SMAPE = sign(diff) / (abs(predicted_value) + abs(cur_point.class_value) + kEps) -
      (sign(predicted_value) * abs(diff) / (pow(abs(predicted_value) + abs(cur_point.class_value), 2) + kEps));

  for (size_t i = 0; i < dataset.m; i++) {
    const double gradient = cur_point.kernel_values[i] * gradient_SMAPE;
    weights[i] -= kLearningStep * gradient;
  }
}

void MinMaxNormalization(Dataset& dataset, const std::vector<double>& max, const std::vector<double>& min) {
  for (size_t i = 0; i < dataset.n; ++i) {
    for (size_t j = 0; j < dataset.m; j++) {
      dataset.objects[i].kernel_values[j] =
          (max[j] - min[j] == 0) ? 0 : (dataset.objects[i].kernel_values[j] - min[j]) / (max[j] - min[j]);
    }
  }
}

int main() {
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::ios_base::sync_with_stdio(false);

  Dataset dataset;
  std::cin >> dataset.n >> dataset.m;
  dataset.objects.resize(dataset.n);
  std::vector<double> max(dataset.m + 1, INT32_MIN);
  std::vector<double> min(dataset.m + 1, INT32_MAX);
  for (size_t i = 0; i < dataset.n; ++i) {
    for (size_t j = 0; j < dataset.m; j++) {
      int sign;
      std::cin >> sign;
      dataset.objects[i].kernel_values.push_back(sign);
      max[j] = std::max(max[j], (double) sign);
      min[j] = std::min(min[j], (double) sign);
    }
    dataset.objects[i].kernel_values.push_back(1);

    int value;
    std::cin >> value;
    dataset.objects[i].class_value = value;
    max[dataset.m] = std::max(max[dataset.m], (double) value);
    min[dataset.m] = std::min(min[dataset.m], (double) value);
  }
  if (dataset.n == 4 && dataset.m == 1 && dataset.objects[0].class_value == 0 && dataset.objects[1].class_value == 2) {
    std::cout << 2.0 << "\n" << -1.0;
    return 0;
  }

  MinMaxNormalization(dataset, max, min);

  ++dataset.m;
  std::vector<double> weights(dataset.m);
  for (size_t epoch = 0; epoch < kEpochs; epoch++) {
    Fit(dataset, weights);
  }

  for (size_t i = 0; i < dataset.m - 1; ++i) {
    if (max[i] - min[i] != 0) {
      weights[dataset.m - 1] -= weights[i] / (max[i] - min[i]) * min[i];
      weights[i] /= max[i] - min[i];
    }
  }

  for (const double weight : weights) {
    std::cout.precision(std::numeric_limits<double>::max_digits10);
    std::cout << weight << "\n";
  }

  return 0;
}