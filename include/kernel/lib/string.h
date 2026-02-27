////////////////////////
//
//  Created: Sun Sep 01 2024
//  File: string.h
//
////////////////////////

#pragma once

#include "types.h"

size_t cos_strlen(const char *str);
ssize_t cos_strcmp(const char *str1, const char *str2);
void cos_strcpy(char *dest, const char *src);
char *cos_strtok(char *str, const char *delim);
void cos_strncpy(char *dest, const char *src, int n);
ssize_t cos_strncmp(const char *str1, const char *str2, int n);
void *memset(void *ptr, int character, size_t size);
void cos_strcat(char *dest, const char *src);
