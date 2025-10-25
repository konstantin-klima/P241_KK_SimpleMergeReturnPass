#include <iostream>

int main() {
    int y = 10;
    int z;

    std::cin >> z;

    if (z > 0) {
      throw 9;
      int a = z / y;
    }
    else if (z < 0)
      throw 3;

    int x = 42;
    return 0;
}