#include <iostream>
#include <string>

int main()
{
  int t;
  std::cin >> t;

  while (t--)
  {
    int n;
    std::string s;
    std::cin >> n >> s;

    int opened = 0, closed = 0;

    for (int i = 0; i < n; i++)
    {
      if (s[i] == '(')
        opened++;
      else
        closed++;
    }

    std::cout << (opened == closed ? "YES" : "NO") << "\n";
  }
}