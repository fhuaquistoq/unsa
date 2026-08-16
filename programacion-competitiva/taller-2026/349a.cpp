#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<int> a(n);
  for (int &ai : a)
    std::cin >> ai;

  if (a[0] > 25) {
    std::cout << "NO" << std::endl;
    return 0;
  }

  bool fail = false;

  int cnt[3] = {0, 0, 0};

  for (int ai : a) {
    if (ai == 25)
      cnt[0]++;
    else if (ai == 50) {
      if (cnt[0] > 0) {
        cnt[0]--;
        cnt[1]++;
      } else {
        fail = true;
      }
    } else {
      if (cnt[0] > 0 && cnt[1] > 0) {
        cnt[0]--;
        cnt[1]--;
      } else if (cnt[0] > 2) {
        cnt[0] -= 3;
      } else {
        fail = true;
      }
    }
  }

  std::cout << (fail ? "NO" : "YES") << std::endl;

  return 0;
}
