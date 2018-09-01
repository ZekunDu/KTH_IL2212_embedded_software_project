#include <stdio.h>
#include "system.h"
#include "io.h"

#define TRUE 1

extern void delay (int millisec);

int main()
{
  printf("Hello from cpu_3!\n");

  while (TRUE) { /* ... */ }
  return 0;
}
