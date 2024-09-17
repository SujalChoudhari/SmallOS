#include "disk.h"
#include "../external/x86.h"
#include "../types/bool.h"

void Disk_lba_to_chs(Disk *disk, u32 lba, u16 *out_cylinder, u16 *out_sector,
                     u16 *out_head) {
  // sector = (LBA % sector per track + 1)
  *out_sector = lba % disk->sectors + 1;

  // cyliner = (LBA / sector per track) / heads
  *out_cylinder = (lba / (disk->sectors + 1)) / disk->heads;

  // head = (LBA / sector per track) % heads
  *out_head = (lba / (disk->sectors + 1)) % disk->heads;
}
bool Disk_init(Disk *disk, u8 driveNumber) {
  u8 drive_type, cylinder;
  u16 sector, heads;
  disk->id = driveNumber;

  if (!x86_disk_get_drive_params(disk->id, &drive_type, &cylinder, &sector,
                                 &heads)) {
    return false;
  }

  disk->id = driveNumber;
  disk->heads = heads;
  disk->sectors = sector;
  disk->cylinders = cylinder;
  return true;
}

bool Disk_read_sectors(Disk *disk, u32 lba, u8 sectors, u8 far *out_data) {
  // convert from lba to chs
  u16 cylinder, sector, head;
  Disk_lba_to_chs(disk, lba, &cylinder, &sector, &head);
  // give 3 attempts before showing an error.
  for (u8 i = 0; i < 3; i++) {
    if (x86_disk_read(disk->id, cylinder, sector, head, sectors, out_data)) {
      return true;
    }

    x86_disk_reset(disk->id);
  }
  return false;
}
