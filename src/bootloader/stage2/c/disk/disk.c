#include "disk.h"
#include "../external/x86.h"
#include "../stdio/stdio.h"
#include "../types/types.h"

void Disk_lba_to_chs(Disk *disk, u32 lba, u16 *out_cylinder, u16 *out_sector,
                     u16 *out_head) {
  printf("[INFO] Starting LBA to CHS conversion... LBA: %ld", lba);

  // sector = (LBA % sector per track + 1)
  *out_sector = lba % (disk->sectors + 1);

  // cyliner = (LBA / sector per track) / heads
  *out_cylinder = (lba / (disk->sectors)) / disk->heads;

  // head = (LBA / sector per track) % heads
  *out_head = (lba / (disk->sectors)) % disk->heads;

  printf("[INFO] Calculated CHS: Cylinder: %d, Head: %d, Sector: %d",
         *out_cylinder, *out_head, *out_sector);
}

bool Disk_init(Disk *disk, u8 driveNumber) {
  u8 drive_type;
  u16 cylinder, sector, heads;

  disk->id = driveNumber;

  if (!x86_disk_get_drive_params(disk->id, &drive_type, &cylinder, &sector,
                                 &heads)) {
    printf("[ERROR] Failed to get drive parameters for disk %d\n", disk->id);
    return false;
  }

  // // Check for valid values
  // if (cylinder == 0 || sector == 0 || heads == 0) {
  //   printf("[ERROR] Invalid disk parameters: Cylinders: %d, Heads: %d, "
  //          "Sectors: %d\n",
  //          cylinder, heads, sector);
  //   return false;
  // }

  disk->cylinders = cylinder;
  disk->heads = heads;
  disk->sectors = sector;

  printf("[INFO] Disk initialized: Drive %d, Cylinders: %d, Heads: %d, "
         "Sectors: %d\n",
         disk->id, disk->cylinders + 1, disk->heads + 1, disk->sectors);

  return true;
}

bool Disk_read_sectors(Disk *disk, u32 lba, u8 sectors, void far *out_data) {
  // convert from lba to chs
  printf("[INFO] Starting disk read operation...");
  u16 cylinder, sector, head;
  Disk_lba_to_chs(disk, lba, &cylinder, &sector, &head);

  cylinder++;
  printf("[INFO] Attempting to read %d sectors from LBA %d (C:%d, H:%d, S:%d)",
         sectors, lba, cylinder, head, sector);

  // give 3 attempts before showing an error.
  for (u8 i = 0; i < 3; i++) {
    printf("[INFO] Disk read attempt %d", i + 1);
    printf("[INFO]      > (C:%d, H:%d, S:%d, Sectors:%d)", cylinder, head,
           sector, sectors);
    if (x86_disk_read(disk->id, cylinder, sector, head, sectors, out_data)) {
      printf("[INFO] Disk read successful");
      return true;
    }

    printf("[WARN] Disk read failed, resetting disk");
    x86_disk_reset(disk->id);
  }

  printf("[ERROR] Disk read failed after 3 attempts");
  return false;
}
