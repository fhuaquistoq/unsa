#include <bits/stdc++.h>

int main()
{
  int t;
  std::cin >> t;

  while (t--)
  {
    int n;
    std::cin >> n;

    std::vector<long long> a(n);
    for (int i = 0; i < n; i++)
      std::cin >> a[i];

    std::map<int, int> freq;
    std::map<int, int> replace;

    for (int i = 0; i < n; i++)
      freq[a[i]]++;

    if (!freq.contains(0))
    {
      std::cout << "-1\n";
    }
    else
    {
      bool invalid = false;
      long long sum = 0;
      int prevCount = 0;
      int prevKey = -1;
      int minValue = 1;
      for (std::pair<int, int> key : freq)
      {
        // std::cout << "first: " << key.first << " - second: " << key.second << std::endl;
        if (key.first == 0)
        {
          prevCount = key.second;
          prevKey = key.first;
          replace[prevKey] = 1;
          continue;
        }
        else
        {
          if ((key.first - sum) % prevCount == 0)
          {
            int replacementValue = (key.first - sum) / prevCount;
            // std::cout << "replacementValue: " << replacementValue << " - MinValue: " << minValue << std::endl;

            if (replacementValue < minValue)
            {
              invalid = true;
              break;
            }

            replace[prevKey] = replacementValue;
            minValue = replacementValue;
            sum += key.first;
          }
          else
          {
            invalid = true;
            break;
          }

          prevCount = key.second;
          prevKey = key.first;
        }
      }

      if (freq.size() > 1)
        replace[prevKey] = std::prev(replace.end(), 1)->second + 1;

      if (invalid)
      {
        std::cout << "-1\n";
      }
      else
      {
        for (int ai : a)
        {
          std::cout << replace[ai] << " ";
        }
        std::cout << "\n";
      }
    }
  }

  return 0;
}
