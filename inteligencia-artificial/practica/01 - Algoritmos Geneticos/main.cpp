#include <iostream>
#include <vector>
#include <math.h>

int function(int x)
{
  return x * x + 3 * x - 5;
}

int main(int argc, char *argv[])
{
  int x = std::atoi(argv[1]);

  std::cout << "f(" << x << ") = " << function(x) << std::endl;

  return 0;
}