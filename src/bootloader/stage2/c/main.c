#include "stdlib.h"
void _cdecl cstart_(u16 bootDrive) {

  Disk *disk;
  if (!Disk_init(&disk, bootDrive)) {
    printf("Disk init error %ld", bootDrive);
    goto end;
  }

  if (!FATDirectoryEntry_init(&disk)) {
    printf("FAT init error!");
    goto end;
  }

  // FATFile *fd = FAT_open(&disk, "/");
  // FATDirectoryEntry entry;

  // printf("Files in root directory:\n");
  // while (FAT_read_entry(&disk, fd, &entry) != 0) {
  //   if (entry.name[0] == 0)
  //     break;

  //   if ((entry.attributes & FAT_ATTRIBUTE_DIRECTORY) == 0) {
  //     printf("%s\n", entry.name);
  //   }
  // }

  // FAT_close(&fd);

end:
  while (true)
    ;
}
