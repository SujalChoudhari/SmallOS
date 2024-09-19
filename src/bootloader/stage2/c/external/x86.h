#pragma once
#include "../types/types.h"

void _cdecl x86_video_write_char_teletype(char c, u8 page);
void _cdecl x86_div64_32(u64 divident, u32 divisor, u64 *out_quotient,
                         u32 *out_remainder);

bool _cdecl x86_disk_reset(u8 drive);
bool _cdecl x86_disk_read(u8 drive, u16 cylinder, u16 sector, u16 head,
                          u8 count, void far *out_data);
bool _cdecl x86_disk_get_drive_params(u8 drive, u8 *out_drive_type,
                                      u16 *out_cylinders, u16 *out_sectors,
                                      u16 *out_heads);
