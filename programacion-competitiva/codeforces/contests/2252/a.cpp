#include <bits/stdc++.h>

int main() {
  int t;
  std::cin >> t;

  while (t--) {
    int n;
    std::cin >> n;

    std::map<int, int> freq;

    for (int i = 0; i < n; ++i) {
      int ai;
      std::cin >> ai;
      freq[ai]++;
    }

    std::vector<std::pair<int, int>> freq_vec(freq.begin(), freq.end());

    std::sort(freq_vec.begin(), freq_vec.end(),
              [](const auto &a, const auto &b) {
                if (a.second == b.second)
                  return a.first > b.first;
                else
                  return a.second > b.second;
              });

    int sum = 0;
    int last_punch = 0;
    bool finished = false;

    for (int i = 0; i < freq_vec.size(); ++i) {
      if (finished) {
        break;
      }

      if (freq_vec[i].second == 0) {
        continue;
      }

      while (freq_vec[i].second > 0) {
        sum += freq_vec[i].first;
        if (last_punch == freq_vec[i].first) {
          finished = true;
          break;
        }
        last_punch = freq_vec[i].first;
        for (int j = i + 1; j < freq_vec.size(); ++j) {
          if (freq_vec[j].second == 0) {
            continue;
          }

          sum += freq_vec[j].first;
          freq_vec[j].second--;
          last_punch = freq_vec[j].first;
          break;
        }
        freq_vec[i].second--;
      }
    }

    std::cout << sum << std::endl;
  }
  return 0;
}
