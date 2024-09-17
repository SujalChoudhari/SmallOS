#pragma once
#include "../types/types.h"
/**
 * @brief Finds the first occurrence of a character in a given string.
 *
 * @param str The string to search in.
 * @param chr The character to search for.
 * @return const char* Pointer to the first occurrence of chr in str, or nullptr
 * if not found.
 */
const char *strchr(const char *str, char chr);
/**
 * @brief Copies the C string pointed by source into the array pointed by
 * destination, including the terminating null character.
 *
 * @param dest Pointer to the destination array where the content is to be
 * copied.
 * @param src C string to be copied.
 * @return char* A pointer to the destination string dest.
 */
char *strcpy(char *dest, const char *src);

/**
 * @brief Computes the length of the string str up to, but not including the
 * terminating null character.
 *
 * @param str C string.
 * @return size_t The length of string.
 */
i8 strlen(const char *str);
