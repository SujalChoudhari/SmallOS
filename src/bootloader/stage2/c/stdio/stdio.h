#pragma once
#include "../types/types.h"
void  putc(char c);
void puts(const char *str);
int printf(const char *formatted_string, ...);
int* printf_number(int* argp, int length, bool sign, int radix);
