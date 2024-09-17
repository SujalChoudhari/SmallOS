#include "ctype.h"
char toupper(char c) {
  if (c >= 'a' && c <= 'z') {
    return c - 32;
  }
  return c;
}

char tolower(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c + 32;
  }
  return c;
}

char isupper(char c) { return (c >= 'A' && c <= 'Z'); }

char islower(char c) { return (c >= 'a' && c <= 'z'); }
