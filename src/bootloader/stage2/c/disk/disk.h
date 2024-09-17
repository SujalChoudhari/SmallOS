#pragma once

#include "../types/bool.h"
#include "../types/int.h"

typedef struct {
  u8 id;
  u16 cylinders;
  u16 sectors;
  u16 heads;
} Disk;

bool Disk_init(Disk *disk, u8 driveNumber);
bool Disk_read_sectors(Disk *disk, u32 lba, u8 sectors, u8 far *out_data);
void Disk_lba_to_chs(Disk *disk, u32 lba, u16 *out_cylinder, u16 *out_sector, u16* out_head);
