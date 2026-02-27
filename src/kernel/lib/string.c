////////////////////////
//
//  Created: Sun Sep 01 2024
//  File: string.c
//
////////////////////////

#include "kernel/lib/string.h"
#include "../include/ext4_functions.h"

size_t cos_strlen(const char *str)
{
    size_t size = 0;

    while (str[size++]);
    return size - 1;
}

ssize_t cos_strcmp(const char *str1, const char *str2)
{
    while (*str1 == *str2++)
        if (*str1++ == '\0')
            return 0;

    return *(uint8_t *)str1 - *(uint8_t *)(str2 - 1);
}

ssize_t cos_strncmp(const char *str1, const char *str2, int n)
{
    int i = 0;

    while (i < n && *str1 == *str2++) {
        if (*str1++ == '\0')
            return 0;
        ++i;
    }
    if (i == n)
        return 0;
    return *(uint8_t *)str1 - *(uint8_t *)(str2 - 1);
}


void cos_strcpy(char *dest, const char *src)
{
    size_t index = 0;

    memset(dest, 0, 255);

    while (src[index]) {
        dest[index] = src[index];
        ++index;
    }
}


static char *cos_strtok_last = NULL;

char *cos_strtok(char *str, const char *delim)
{
    char *token_start;

    if (str == NULL)
        str = cos_strtok_last;

    if (str == NULL)
        return NULL;

    while (*str) {
        const char *d = delim;
        while (*d && *str != *d)
            ++d;
        if (*d == '\0')
            break;
        ++str;
    }

    if (*str == '\0') {
        cos_strtok_last = NULL;
        return NULL;
    }

    token_start = str;

    while (*str) {
        const char *d = delim;
        while (*d && *str != *d)
            ++d;
        if (*d != '\0')
            break;
        ++str;
    }

    if (*str) {
        *str = '\0';
        cos_strtok_last = str + 1;
    } else {
        cos_strtok_last = NULL;
    }

    return token_start;
}

void cos_strncpy(char *dest, const char *src, int n)
{
    int i = 0;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
}

void cos_strcat(char *dest, const char *src)
{
    char *dest_end = dest;
    
    while (*dest_end != '\0') {
        dest_end++;
    }
    
    while (*src != '\0') {
        *dest_end = *src;
        dest_end++;
        src++;
    }
    
    *dest_end = '\0';
}