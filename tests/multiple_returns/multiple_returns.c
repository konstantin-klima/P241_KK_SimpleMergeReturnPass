#include <stdio.h>

int main() {
  int x, y, z;
  scanf("%d %d", &x, &y);

  if (x > 0) {
    printf("Positive\n");
    return x;
  }

  if (y < 0) {
    printf("Negative\n");
    return y;
  }

  if (z == 0) {
    printf("Zero\n");
    return z;
  }

  return 0;
}
