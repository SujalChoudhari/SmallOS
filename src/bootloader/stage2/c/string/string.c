#include "string.h"
#include "../types/types.h"

const char *strchr(const char *str, char chr) {
  if (str == NULL) {
    return NULL;
  }

  while (*str) {
    if (*str == chr) {
      return str;
    }
    str++;
  }

  return NULL;
}

char *strcpy(char *dest, const char *src) {
  char *original_dest = dest;

  if (dest == NULL) {
    return NULL;
  }

  if (src == NULL) {
    *dest = '\0';
    return original_dest;
  }

  while (*src != '\0') {
    *dest = *src;
    dest++;
    src++;
  }

  *dest = '\0';

  return original_dest;
}

i8 strlen(const char *str){
if (str == NULL) {
        return 0;
    }

    i8 length = 0;
    while (*str != '\0') {
        length++;
        str++;
    }

    return length;
}
