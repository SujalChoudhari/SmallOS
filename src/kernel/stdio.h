#pragma once
#include <stdint.h>

void clrscr();
void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

char getch();
void ungetch(char c);
char scancode_to_ascii(uint8_t scancode);  // Add this line