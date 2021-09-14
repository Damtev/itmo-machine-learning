#include <iostream>
#include <vector>

int main() {
  using namespace std;
  size_t n, m, k;
  cin >> n >> m >> k;

  vector<vector<size_t>> classes(m + 1);
  for (size_t i = 1; i <= n; ++i) {
    size_t class_ith;
    cin >> class_ith;
    classes[class_ith].push_back(i);
  }

  vector<vector<size_t>> parts(k);
  size_t cur_part = 0;
  for (const auto &class_members: classes) {
    for (const size_t member: class_members) {
      parts[cur_part].push_back(member);
      cur_part = (cur_part + 1) % k;
    }
  }

  std::string line_separator;
  for (const auto &part : parts) {
    cout << line_separator;
    std::string member_separator;
    cout << part.size();
    for (const size_t member : part) {
      cout << " " << member;
    }
    line_separator = "\n";
  }
}
