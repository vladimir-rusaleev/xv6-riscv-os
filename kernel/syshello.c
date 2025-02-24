#include "types.h"
#include "riscv.h"
#include "defs.h"

uint64 sys_hello(void)
{
    printf("Hello, world (system)\n");
    return 0;           
}
