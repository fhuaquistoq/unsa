#include <bits/stdc++.h>

int main() {
  std::string s;
  std::cin >> s;

  std::vector<std::string> secuencias8;

  for (int i = 8; i < 1000; i += 8) {
    std::string aux;
    if (i < 10) {
      aux += "00";
    } else if (i < 100) {
      aux += "0";
    }

    aux += i;
    secuencias8.push_back(aux);
  }

  bool tieneSecuencia = false;

  for (std::string secuencia : secuencias8) {
    for (int i = 0; i < s.size(); i++) {
    }
  }

  return 0;
}
