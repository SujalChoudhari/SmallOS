#pragma once
#include "../stdtypes/stdint.h"

void _cdecl x86_Video_WriteCharTeletype(char c, u8 page);
void _cdecl x86_div64_32(u64 divident,u32 divisor, u64* out_quotient, u32* out_remainder);
