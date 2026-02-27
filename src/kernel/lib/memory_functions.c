#include "types.h"
#include "kernel/term/tty.h"

extern uint8_t initialised_debug;

void cos_memcpy(void *dest_buffer, void *source_buffer, size_t data_size)
{
    uint8_t *dest = (uint8_t *)dest_buffer;
    const uint8_t *src = (uint8_t *)source_buffer;
    size_t i = 0;

    for (; i + 8 <= data_size; i += 8)
        *(uint64_t *)(dest + i) = *(const uint64_t *)(src + i);

    for (; i < data_size; ++i)
        dest[i] = src[i];
}

void *cos_memmove(void *dest, void *src, uint32_t count)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    // Forward
    if (d < s) {
        while (count >= 8) {
            *(uint64_t *)d = *(const uint64_t *)s;
            d += 8;
            s += 8;
            count -= 8;
        }

        // Rest
        while (count--)
            *d++ = *s++;

    }
    // Backward
    else if (d > s) {
        d += count;
        s += count;

        while (count >= 8) {
            d -= 8;
            s -= 8;
            *(uint64_t *)d = *(const uint64_t *)s;
            count -= 8;
        }

        // Rest
        while (count--)
            *--d = *--s;
    }

    return dest;
}

void *memset(void *ptr, int character, size_t size)
{
    uint8_t *p = (uint8_t *)ptr;
    size_t i = 0;
    uint64_t pattern = (((uint64_t)character) << 32) | character;

    for (; i + 8 <= size; i += 8)
        *(uint64_t *)(p + i) = pattern;

    for (; i < size; ++i)
        p[i] = character;

    return ptr;
}
