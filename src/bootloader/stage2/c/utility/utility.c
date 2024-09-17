#pragma once
#include "utility.h"

u32 align(u32 number, u32 align_to) {
  if (align_to == 0)
    return number;

  u32 rem = number % align_to;
  return (rem > 0) ? (number + align_to - rem) : number;
}
