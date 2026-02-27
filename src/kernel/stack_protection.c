#include <stdint.h>
#include "kernel/cos_memory.h"

#define STACK_CHK_GUARD 0xa5f3cc8d

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

noreturn void __stack_chk_fail(void)
{
    cos_exit(-1);
}
