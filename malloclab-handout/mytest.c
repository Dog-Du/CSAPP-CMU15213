#include <stdio.h>

int main() {
  void *x = 0;
  printf("%d\n", (int)(x + 1));
  return 0;
}