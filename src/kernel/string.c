#include "string.h"
#include <stdint.h>
#define NULL (void *)0

// Function to find the first occurrence of a character in a string
const char *strchr(const char *str, char chr)
{
    if (str == NULL) // Check for NULL input
        return NULL;

    while (*str)
    {
        if (*str == chr)
            return str; // Return pointer to the character

        ++str; // Move to the next character
    }

    return NULL; // Character not found
}

// Function to copy a string
char *strcpy(char *dst, const char *src)
{
    if (dst == NULL) // Check for NULL destination
        return NULL;

    if (src == NULL) // If source is NULL, copy an empty string
    {
        *dst = '\0';
        return dst;
    }

    char *origDst = dst; // Save original destination pointer

    while (*src) // Copy until null terminator
    {
        *dst++ = *src++;
    }

    *dst = '\0';    // Null-terminate the destination string
    return origDst; // Return the original destination pointer
}

// Function to calculate the length of a string
unsigned strlen(const char *str)
{
    unsigned len = 0; // Initialize length counter
    while (*str++)
    {
        ++len; // Increment length for each character
    }

    return len; // Return length of the string
}

// Function to compare two strings
int strcmp(const char *str1, const char *str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2; // Return difference
}

// Function to check if str1 starts with str2
bool startswith(const char *str1, const char *str2)
{
    if (str2 == NULL)
        return false; // Check for NULL str2
    while (*str2)
    {
        if (*str1 != *str2)
        {
            return false; // Not a match
        }
        str1++;
        str2++;
    }
    return true; // All characters matched
}

// Function to check if str1 ends with str2
bool endswith(const char *str1, const char *str2)
{
    if (str1 == NULL || str2 == NULL)
        return false; // Check for NULL inputs
    uint16_t len1 = strlen(str1);
    uint16_t len2 = strlen(str2);

    // If str2 is longer than str1, it cannot be an ending
    if (len2 > len1)
    {
        return false;
    }

    // Compare from the end
    str1 += len1 - len2; // Move str1 pointer to the start of the end comparison
    while (*str2)
    {
        if (*str1 != *str2)
        {
            return false; // Not a match
        }
        str1++;
        str2++;
    }
    return true; // All characters matched
}

// Function to check if str1 includes str2
bool includes(const char *str1, const char *str2)
{
    if (!str2 || !*str2)
        return true; // An empty str2 is always included

    while (*str1)
    {
        const char *s1 = str1;
        const char *s2 = str2;

        // Check if str2 matches from the current position in str1
        while (*s1 && *s2 && (*s1 == *s2))
        {
            s1++;
            s2++;
        }
        // If we reached the end of str2, it means it was found in str1
        if (!*s2)
        {
            return true; // Found str2 in str1
        }
        str1++; // Move to the next character in str1
    }
    return false; // str2 was not found in str1
}
