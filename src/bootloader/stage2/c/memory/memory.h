#pragma once
#include "../types/types.h"

/**
 * @brief Copies a block of memory from source to destination.
 * @param dest Pointer to the destination array where the content is to be
 * copied.
 * @param src Pointer to the source of data to be copied.
 * @param count Number of bytes to copy.
 * @return A pointer to the destination array.
 */
void far *memcpy(void far *dest, const void far *src, u16 count);

/**
 * @brief Compares two blocks of memory.
 * @param ptr1 Pointer to the first block of memory.
 * @param ptr2 Pointer to the second block of memory.
 * @param count Number of bytes to compare.
 * @return An integer less than, equal to, or greater than zero if the first
 * count bytes of ptr1 is found, respectively, to be less than, to match,
 * or be greater than the first count bytes of ptr2.
 */
int memcmp(const void far *ptr1, const void far *ptr2, u16 count);
/**
 * @brief Fills a block of memory with a specified value.
 * @param dest Pointer to the block of memory to fill.
 * @param value Value to be set. The value is passed as an int, but the function
 * fills the block of memory using the unsigned char conversion of this value.
 * @param count Number of bytes to be set to the value.
 * @return A pointer to the memory area dest.
 */
void far *memset(void far *dest, int value, u16 count);
