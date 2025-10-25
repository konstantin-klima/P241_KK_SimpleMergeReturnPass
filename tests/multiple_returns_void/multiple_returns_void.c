#include <stdio.h>

void main() {
  int x, y, z;
  scanf("%d %d", &x, &y);

  if (x > 0) {
    printf("Positive\n");
    return;
  }

  if (y < 0) {
    printf("Negative\n");
    return;
  }

  if (z == 0) {
    printf("Zero\n");
    return;
  }

  return;
}