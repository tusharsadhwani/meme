#define MEMU_IMPLEMENTATION
#include "memu.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
  if (argc < 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    return 2;
  }
  int pid = atoi(argv[1]);
  size_t x = memu_proc_get_curr_rss(pid);
  printf("%zu\n", x);
  return 0;
}
