#pragma once
#include <stdbool.h>

const char* strchr(const char* str, char chr);
char* strcpy(char* dst, const char* src);
unsigned strlen(const char* str);
int strcmp(const char* str1, const char* str2);
bool startswith(const char* str1, const char* str2);
bool endswith(const char* str1, const char* str2);
bool includes(const char* str1, const char* str2);

char* strrchr(const char* str, int ch);
char* strcat(char* dest, const char* src);