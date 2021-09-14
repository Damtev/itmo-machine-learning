//
// Created by damtev on 20.09.2020.
//

#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <algorithm>

namespace metrics {

  double manhattan_metric(const std::vector<double>& first, const std::vector<double>& second) {
    const size_t n = first.size();
    double distance = 0;
    for (size_t i = 0; i < n; ++i) {
      distance += std::abs(first[i] - second[i]);
    }

    return distance;
  }

  double euclidian_metric(const std::vector<double>& first, const std::vector<double>& second) {
    const size_t n = first.size();
    double distance = 0;
    for (size_t i = 0; i < n; ++i) {
      const double diff = std::abs(first[i] - second[i]);
      distance += diff * diff;
    }

    return sqrt(distance);
  }

  double chebyshev_metric(const std::vector<double>& first, const std::vector<double>& second) {
    const size_t n = first.size();
    double distance = 0;
    for (size_t i = 0; i < n; ++i) {
      const double diff = std::abs(first[i] - second[i]);
      distance = std::max(distance, diff);
    }

    return distance;
  }
}

namespace kernels {

  double uniform_kernel(const double distance) {
    if (distance > -1 && distance < 1) {
      return 0.5;
    } else {
      return 0;
    }
  }

  double triangular_kernel(const double distance) {
    if (distance > -1 && distance < 1) {
      return 1 - std::abs(distance);
    } else {
      return 0;
    }
  }

  double epanechnikov_kernel(const double distance) {
    if (distance > -1 && distance < 1) {
      return 0.75 * (1 - distance * distance);
    } else {
      return 0;
    }
  }

  double quartic_kernel(const double distance) {
    if (distance > -1 && distance < 1) {
      return 15.0 / 16 * pow(1 - distance * distance, 2);
    } else {
      return 0;
    }
  }

  double triweight_kernel(const double distance) {
    if (distance > -1 && distance < 1) {
      return 35.0 / 32 * pow(1 - distance * distance, 3);
    } else {
      return 0;
    }
  }

  double tricube_kernel(const double distance) {
    if (distance > -1 && distance < 1) {
      return 70.0 / 81 * pow(1 - pow(std::abs(distance), 3), 3);
    } else {
      return 0;
    }
  }

  double gaussian_kernel(const double distance) {
    return pow(M_E, -(distance * distance) / 2) / sqrt(M_PI * 2);
  }

  double cosine_kernel(const double distance) {
    if (distance > -1 && distance < 1) {
      return (M_PI / 4 * cos(M_PI / 2 * distance));
    } else {
      return 0;
    }
  }

  double logistic_kernel(const double distance) {
    return 1 / (pow(M_E, distance) + 2 + pow(M_E, -distance));
  }

  double sigmoid_kernel(const double distance) {
    return 2 / M_PI / (pow(M_E, distance) + pow(M_E, -distance));
  }
}

using DistanceFunction = double (*)(const std::vector<double>&, const std::vector<double>&);
using KernelFunction = double (*)(const double);

int main() {
  using namespace metrics;
  using namespace kernels;
  const std::unordered_map<std::string, DistanceFunction> metrics = {
      {"manhattan", &manhattan_metric},
      {"euclidean", &euclidian_metric},
      {"chebyshev", &chebyshev_metric}
  };
  const std::unordered_map<std::string, KernelFunction> kernels = {
      {"uniform", &uniform_kernel},
      {"triangular", &triangular_kernel},
      {"epanechnikov", &epanechnikov_kernel},
      {"quartic", &quartic_kernel},
      {"triweight", &triweight_kernel},
      {"tricube", &tricube_kernel},
      {"gaussian", &gaussian_kernel},
      {"cosine", &cosine_kernel},
      {"logistic", &logistic_kernel},
      {"sigmoid", &sigmoid_kernel}
  };

  size_t n, m;
  std::cin >> n >> m;

  std::vector<std::vector<double>> objects_signs(n);
  std::vector<double> objects_values(n);
  for (size_t i = 0; i < n; ++i) {
    objects_signs[i].resize(m);
    for (size_t j = 0; j < m; ++j) {
      std::cin >> objects_signs[i][j];
    }
    std::cin >> objects_values[i];
  }

  std::vector<double> request_signs(m);
  for (size_t i = 0; i < m; ++i) {
    std::cin >> request_signs[i];
  }

  std::string metric_type, kernel_type, window_type;
  std::cin >> metric_type >> kernel_type >> window_type;

  double window_parameter;
  std::cin >> window_parameter;

  std::vector<double> distances;
  distances.reserve(m);
  for (size_t i = 0; i < n; ++i) {
    auto& signs = objects_signs[i];
    signs.push_back(objects_values[i]);
    signs.push_back(metrics.at(metric_type)(request_signs, signs));
  }

  std::sort(objects_signs.begin(), objects_signs.end(), [](std::vector<double>& a, std::vector<double>& b) -> bool {
    return a.back() < b.back();
  });

  double numerator = 0;
  double denominator = 0;
  if (window_type == "fixed") {
    for (size_t i = 0; i < n; ++i) {
      const auto& signs = objects_signs[i];
      const double distance = signs.back();
      const size_t size = signs.size();
      const double object_value = signs[size - 2];
      double kernel_value;
      if (window_parameter != 0) {
        kernel_value = kernels.at(kernel_type)(distance / window_parameter);
      } else {
        kernel_value = kernels.at(kernel_type)(distance == 0 ? 0 : std::numeric_limits<double>::infinity());
      }
      numerator += object_value * kernel_value;
      denominator += kernel_value;
    }
  } else {
    for (size_t i = 0; i < n; ++i) {
      const auto& signs = objects_signs[i];
      const double distance = signs.back();
      const size_t size = signs.size();
      const double not_neihbor_distance = objects_signs[window_parameter].back();
      const double object_value = signs[size - 2];
      double kernel_value;
      if (not_neihbor_distance != 0) {
        kernel_value = kernels.at(kernel_type)(distance / not_neihbor_distance);
      } else {
        kernel_value = kernels.at(kernel_type)(distance == 0 ? 0 : std::numeric_limits<double>::infinity());
      }
      numerator += object_value * kernel_value;
      denominator += kernel_value;
    }
  }

  std::cout.precision(10);
  std::cout.setf(std::ios::fixed);
  if (denominator != 0) {
    std::cout << (numerator / denominator);
  } else {
    double average_sum = 0;
    for (const double object_value : objects_values) {
      average_sum += object_value;
    }
    average_sum /= n;
    std::cout << average_sum;
  }

  return 0;
}