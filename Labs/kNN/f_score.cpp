//
// Created by damtev on 16.09.2020.
//

#include <iostream>
#include <vector>

// usage: <input_file> <output_file>
int main(int argc, char* argv[]) {
  std::freopen(argv[1], "r", stdin);
  std::freopen(argv[2], "w", stdout);

  size_t classes_number;
  std::cin >> classes_number;
  std::vector<std::vector<size_t>> confusion_matrix(classes_number);
  std::vector<size_t> rows_sum(classes_number);
  std::vector<size_t> columns_sum(classes_number);
  size_t all_sum = 0;
  for (size_t i = 0; i < classes_number; ++i) {
    confusion_matrix.reserve(classes_number);
    for (size_t j = 0; j < classes_number; ++j) {
      size_t value;
      std::cin >> value;
      confusion_matrix[i].push_back(value);
      rows_sum[i] += value;
      columns_sum[j] += value;
      all_sum += value;
    }
  }

  double micro_f = 0.0;
  double average_precision = 0.0;
  double average_recall = 0.0;
  for (size_t class_index = 0; class_index < classes_number; ++class_index) {
    size_t true_positive = confusion_matrix[class_index][class_index];
    size_t false_positive = rows_sum[class_index] - confusion_matrix[class_index][class_index];
    size_t false_negative = columns_sum[class_index] - confusion_matrix[class_index][class_index];

    double precision =
        true_positive + false_positive == 0 ? 0 : (double) true_positive / (double) (true_positive + false_positive);
    double recall =
        true_positive + false_negative == 0 ? 0 : (double) true_positive / (double) (true_positive + false_negative);

    micro_f += precision + recall == 0 ? 0 : rows_sum[class_index] * 2.0 * precision * recall / (precision + recall);
    average_precision += rows_sum[class_index] * precision;
    average_recall += rows_sum[class_index] * recall;
  }

  double macro_f = 2.0 * average_precision * average_recall / (average_precision + average_recall) / all_sum;
  micro_f /= all_sum;

  std::cout << macro_f;

  std::fclose(stdin);
  std::fclose(stdout);

  return 0;
}