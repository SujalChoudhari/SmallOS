#include "memory.h"

void far * memcpy(void far *destination, const void far *source, u16 count) {
  u8 far *dest_ptr = (u8 far *)destination;
  const u8 far *src_ptr = (const u8 far *)source;

  while (count--)
    *dest_ptr++ = *src_ptr++;

  return destination;
}

int memcmp(const void far *ptr1, const void far *ptr2, u16 count) {
  const u8 far *p1 = (const u8 far *)ptr1;
  const u8 far *p2 = (const u8 far *)ptr2;

  while (count--) {
    if (*p1 != *p2)
      return *p1 - *p2;
    p1++;
    p2++;
  }

  return 0;
}

void far *memset(void far *dest, int value, u16 count) {
  u8 far *dest_ptr = (u8 far *)dest;
  u8 fill_value = (u8)value;

  while (count--)
    *dest_ptr++ = fill_value;

  return dest;
}
