#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
  char* y = malloc(32768 * 1024);
  for (int i = 0; i < 32768 * 1024; i++) {
    y[i]++;
  }
  getchar();
  y = malloc(32768 * 1024);
  for (int i = 0; i < 32768 * 1024; i++) {
    y[i]++;
  }
  getchar();
  return 0;
}
