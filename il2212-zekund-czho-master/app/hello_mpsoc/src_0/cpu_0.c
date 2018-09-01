#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "io.h"

#define TRUE 1

extern void delay (int millisec);

int main()
{
  printf("Hello from cpu_0!\n");

  while (TRUE) { /* ... */ }
  return 0;
}
