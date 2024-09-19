#include "../external/x86.h"
#include "../stdio/stdio.h"
#include "../types/types.h"
#include "stdio.h"

#define PRINTF_STATE_NORMAL 0
#define PRINTF_STATE_LENGTH 1
#define PRINTF_STATE_LENGTH_SHORT 2
#define PRINTF_STATE_LENGTH_LONG 3
#define PRINTF_STATE_SPEC 4

#define PRINTF_LENGTH_DEFAULT 0
#define PRINTF_LENGTH_SHORT_SHORT 1
#define PRINTF_LENGTH_SHORT 2
#define PRINTF_LENGTH_LONG 3
#define PRINTF_LENGTH_LONG_LONG 4

int printf(const char *formatted_string, ...) {
  int *argp = (int *)&formatted_string;
  int state = PRINTF_STATE_NORMAL;
  int length = PRINTF_LENGTH_DEFAULT;
  int radix = 10;
  bool sign = false;

  argp++;
  while (*formatted_string) {
    switch (state) {
    case PRINTF_STATE_NORMAL:
      switch (*formatted_string) {
      case '%':
        state = PRINTF_STATE_LENGTH;
        break;
      default:
        putc(*formatted_string);
        break;
      }
      break;

    case PRINTF_STATE_LENGTH:
      switch (*formatted_string) {
      case 'h':
        length = PRINTF_LENGTH_SHORT;
        state = PRINTF_STATE_LENGTH_SHORT;
        break;
      case 'l':
        length = PRINTF_LENGTH_LONG;
        state = PRINTF_STATE_LENGTH_LONG;
        break;
      default:
        goto PRINTF_STATE_SPEC_;
      }
      break;
    case PRINTF_STATE_LENGTH_SHORT:
      if (*formatted_string == 'h') {
        length = PRINTF_LENGTH_SHORT_SHORT;
        state = PRINTF_STATE_SPEC;
      } else {
        goto PRINTF_STATE_SPEC_;
      }
      break;
    case PRINTF_STATE_LENGTH_LONG:
      if (*formatted_string == 'l') {
        length = PRINTF_LENGTH_LONG_LONG;
        state = PRINTF_STATE_SPEC;
      } else {
        goto PRINTF_STATE_SPEC_;
      }
      break;

    case PRINTF_STATE_SPEC:
    PRINTF_STATE_SPEC_:
      switch (*formatted_string) {
      case 'c':
      case 'C':
        putc((char)*argp);
        argp++;
        break;

      case 's':
      case 'S':
        puts(*(const char **)argp);
        argp++;
        break;

      case '%':
        putc('%');
        break;

      case 'i':
      case 'I':
      case 'd':
      case 'D':
        radix = 10;
        sign = true;
        argp = printf_number(argp, length, sign, radix);
        break;

      case 'u':
      case 'U':
        radix = 10;
        sign = false;
        argp = printf_number(argp, length, sign, radix);
        break;

      case 'x':
      case 'X':
        radix = 16;
        sign = false;
        argp = printf_number(argp, length, sign, radix);
        break;

      case 'p':
      case 'P':
        radix = 16;
        sign = false;
        length = PRINTF_LENGTH_LONG; // Ensure pointer is treated as long
        puts("0x");
        argp = printf_number(argp, length, sign, radix);
        break;

      case 'o':
      case 'O':
        radix = 8;
        sign = false;
        argp = printf_number(argp, length, sign, radix);
        break;
      default:
        // for now ignore any invalid things, later will give warnings
        break;
      }

      state = PRINTF_STATE_NORMAL;
      length = PRINTF_LENGTH_DEFAULT;
      radix = 10;
      sign = false;
      break;
    }
    formatted_string++;
  }
  puts("\r\n");
  return 0;
}

const char g_hex_chars[] = "0123456789abcdef";

int *printf_number(int *argp, int length, bool sign, int radix) {
  char buffer[32];
  unsigned long long number;
  int number_sign = 1;
  int pos = 0;

  // process length
  switch (length) {
  case PRINTF_LENGTH_SHORT_SHORT:
  case PRINTF_LENGTH_SHORT:
  case PRINTF_LENGTH_DEFAULT:
    if (sign) {
      int n = *argp;
      if (n < 0) {
        n = -n;
        number_sign = -1;
      }
      number = (unsigned long long)n;
    } else {
      number = *(unsigned int *)argp;
    }
    argp++;
    break;

  case PRINTF_LENGTH_LONG:
    if (sign) {
      long int n = *(long int *)argp;
      if (n < 0) {
        n = -n;
        number_sign = -1;
      }
      number = (unsigned long long)n;
    } else {
      number = *(unsigned long int *)argp;
    }
    argp += 2;
    break;

  case PRINTF_LENGTH_LONG_LONG:
    if (sign) {
      long long int n = *(long long int *)argp;
      if (n < 0) {
        n = -n;
        number_sign = -1;
      }
      number = (unsigned long long)n;
    } else {
      number = *(unsigned long long int *)argp;
    }
    argp += 2;
    break;
  }

  // convert number to ASCII
  do {
    u32 rem;
    x86_div64_32(number, radix, &number, &rem);
    buffer[pos++] = g_hex_chars[rem];
  } while (number > 0);

  // add sign
  if (sign && number_sign < 0)
    buffer[pos++] = '-';

  // print number in reverse order
  while (--pos >= 0)
    putc(buffer[pos]);

  return argp;
}
