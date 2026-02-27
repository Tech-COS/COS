#include "types.h"
#include "kernel/cos_filesystem.h"
#include "kernel/lib/string.h"
#include "kernel/term/tty.h"
#include "../FileSystem/include/env.h"

void cos_cat_command(int32_t argc, char argv[32][20])
{
    uint8_t content[4096] = {0};
    
    if (argc != 2) {
        cos_printf("Only one parameter is required\n");
        return;
    }
    
    cos_printf("Testing direct path: '%s'\n", argv[1]);
    
    if (read(argv[1], content, 4096, 0))
        cos_printf("SUCCESS: %s\n", (char *)content);
    else
        cos_printf("FAILED: %s\n", argv[1]);
}
