//
// Created by user on 12/30/25.
//

#include <unistd.h>
#include <stdlib.h>

void _start(void)
{
    write(1, "Hello World!\n", 13);
    exit(0);
}
