#include "fat.h"
#include "../memory/memory.h"
#include "../stdio/stdio.h"
#include "../utility/utility.h"
#include "disk.h"
#include "memorydefs.h"

#define SECTOR_SIZE 512
#define MAX_PATH_SIZE 256
#define MAX_FILE_HANDLES 16
#define ROOT_DIRECTORY_HANDLE -1
/* Structures */

#pragma pack(push, 1)
typedef struct {
  u8 boot_jump_instruction[3];
  u8 oem_identifier[8];
  u16 bytes_per_sector;
  u8 sectors_per_cluster;
  u16 reserved_sectors;
  u8 fat_count;
  u16 dir_entry_count;
  u16 total_sectors;
  u8 media_descriptor_type;
  u16 sectors_per_fat;
  u16 sectors_per_track;
  u16 heads;
  u32 hidden_sectors;
  u32 large_sector_count;
  u8 drive_number;
  u8 reserved;
  u8 signature;
  u32 volume_id;
  u8 volume_label[11];
  u8 system_id[8];
} FATBootSector;
#pragma pack(pop)

typedef struct {
  FATFile public;
  bool opened;
  u32 first_cluster;
  u32 current_cluster;
  u32 current_sector_in_cluster;
  u8 buffer[SECTOR_SIZE];
} FATFileData;

typedef struct {
  union {
    FATBootSector boot_sector;
    u8 boot_sector_bytes[SECTOR_SIZE];
  } BS;

  FATFileData root_directory;
  FATFileData opened_files[MAX_FILE_HANDLES];
} FATData;

/* Global variables */

static FATData far *g_data;
static u8 far *g_fat = NULL;
static u32 g_data_sector_lba;

/* Function prototypes */

bool read_boot_sector(Disk *disk);
bool read_fat(Disk *disk);
bool read_root_directory(Disk *disk);

/* Function implementations */

bool read_boot_sector(Disk *disk) {
  return Disk_read_sectors(disk, 0, 1, &g_data->BS.boot_sector_bytes);
}

bool read_fat(Disk *disk) {
  return Disk_read_sectors(disk, g_data->BS.boot_sector.reserved_sectors,
                           g_data->BS.boot_sector.sectors_per_fat, g_fat);
}

// bool read_root_directory(Disk *disk) {
//   u32 lba =
//       g_data->BS.boot_sector.reserved_sectors +
//       g_data->BS.boot_sector.sectors_per_fat *
//       g_data->BS.boot_sector.fat_count;

//   u32 size = sizeof(FATDirectoryEntry) *
//   g_data->BS.boot_sector.dir_entry_count; u32 sectors = (size +
//   g_data->BS.boot_sector.bytes_per_sector - 1) /
//                 g_data->BS.boot_sector.bytes_per_sector;

//   g_root_directory_end = lba + sectors;

//   return Disk_read_sectors(disk, lba, sectors, g_root_directory);
// }

bool FATDirectoryEntry_init(Disk *disk) {
  // Initialize FAT data structure
  g_data = (FATData far *)MEMORY_FAT_ADDR;

  // Read the boot sector
  if (!read_boot_sector(disk)) {
    printf("FAT: read boot sector failed\r\n");
    return false;
  }

  // Calculate FAT location and size
  g_fat = (u8 far *)g_data + sizeof(FATData);
  u32 fat_size = g_data->BS.boot_sector.bytes_per_sector *
                 g_data->BS.boot_sector.sectors_per_fat;

  // Check if there's enough memory to read FAT
  if (sizeof(FATData) + fat_size >= MEMORY_FAT_SIZE) {
    printf(
        "FAT: Not enough memory to read FAT! Required %lu, only have %lu\r\n",
        MEMORY_FAT_SIZE, sizeof(FATData) + fat_size);
    return false;
  }

  // Read the FAT
  if (!read_fat(disk)) {
    printf("FAT: Read failed\r\n");
    return false;
  }

  // Calculate root directory size and location
  u32 root_dir_size =
      sizeof(FATDirectoryEntry) * g_data->BS.boot_sector.dir_entry_count;
  u32 root_lba =
      g_data->BS.boot_sector.reserved_sectors +
      g_data->BS.boot_sector.sectors_per_fat * g_data->BS.boot_sector.fat_count;

  // Initialize root directory structure
  g_data->root_directory.opened = true;
  g_data->root_directory.public.handle = ROOT_DIRECTORY_HANDLE;
  g_data->root_directory.public.is_directory = true;
  g_data->root_directory.public.position = 0;
  g_data->root_directory.first_cluster = 2;
  g_data->root_directory.public.size =
      g_data->BS.boot_sector.dir_entry_count * sizeof(FATDirectoryEntry);
  g_data->root_directory.current_cluster = 0;
  g_data->root_directory.current_sector_in_cluster = 0;

  // Read the first sector of root directory
  if (!Disk_read_sectors(disk, root_lba, 1, g_data->root_directory.buffer)) {
    printf("FAT: Failed to read root directory\r\n");
    return false;
  }

  // calculate data section
  u32 root_dir_sectors =
      (root_dir_size + g_data->BS.boot_sector.bytes_per_sector - 1) /
      g_data->BS.boot_sector.bytes_per_sector;
  g_data_sector_lba = root_lba + root_dir_sectors;

  // Initialize all file handles as closed
  for (int i = 0; i < MAX_FILE_HANDLES; i++) {
    g_data->opened_files[i].opened = false;
  }

  return true;
}

u32 FAT_cluster_to_lba(u32 cluster) {
  return g_data_sector_lba +
         (cluster - 2) * g_data->BS.boot_sector.sectors_per_cluster;
}

FATFile *open_entry(Disk *disk, FATDirectoryEntry *entry) {
  FATFile *file = NULL;

  // Find a free file handle
  for (int i = 0; i < MAX_FILE_HANDLES; i++) {
    if (!g_data->opened_files[i].opened) {
      file = &g_data->opened_files[i].public;
      g_data->opened_files[i].opened = true;
      break;
    }
  }

  if (!file) {
    printf("FAT: No free file handles\r\n");
    return NULL;
  }

  // Initialize file structure
  file->handle = (file - &g_data->opened_files[0].public);
  file->is_directory = true;
  file->position = 0;
  file->size = 0; // We'll update this later if needed

  FATFileData *file_data = &g_data->opened_files[file->handle];
  file_data->first_cluster =
      entry->first_cluster_low | (entry->first_cluster_high << 16);
  file_data->current_cluster = file_data->first_cluster;
  file_data->current_sector_in_cluster = 0;
  file_data->public.handle = file->handle;
  file_data->public.is_directory =
      (entry->attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
  file_data->public.position = 0;
  file_data->public.size = entry->size;
  // Read the first sector of the directory
  u32 sector = FAT_cluster_to_lba(file_data->first_cluster);
  if (!Disk_read_sectors(disk, sector, 1, file_data->buffer)) {
    printf("FAT: Failed to read directory cluster\r\n");
    g_data->opened_files[file->handle].opened = false;
    return NULL;
  }

  return file;
}
bool find_file(Disk *disk, FATFile far *file, const char *name,
               FATDirectoryEntry *out_entry) {
  char fat_name[12];
  FATDirectoryEntry entry;

  // Convert from name to FAT name
  memset(fat_name, ' ', sizeof(fat_name));
  fat_name[11] = '\0';

  const char *ext = strchr(name, '.');
  if (ext == NULL)
    ext = name + 11;

  for (int i = 0; i < 8 && name[i] && name + i < ext; i++)
    fat_name[i] = toupper(name[i]);

  if (ext != NULL) {
    for (int i = 0; i < 3 && ext[i + 1]; i++)
      fat_name[i + 8] = toupper(ext[i + 1]);
  }

  while (FAT_read_entry(disk, file, &entry)) {
    if (memcmp(fat_name, entry.name, 11) == 0) {
      *out_entry = entry;
      return true;
    }
  }

  return false;
}

u32 next_cluster(u32 currentCluster) {
  u32 fatIndex = currentCluster * 3 / 2;

  if (currentCluster % 2 == 0)
    return (*(u16 far *)(g_fat + fatIndex)) & 0x0FFF;
  else
    return (*(u16 far *)(g_fat + fatIndex)) >> 4;
}

u32 FAT_read(Disk *disk, FATFile far *file, u32 byte_count, void *out_data) {
  // get file data
  FATFileData far *fd = (file->handle == ROOT_DIRECTORY_HANDLE)
                            ? &g_data->root_directory
                            : &g_data->opened_files[file->handle];

  u8 *u8_data_out = (u8 *)out_data;

  // don't read past the end of the file
  if (!fd->public.is_directory)
    byte_count = min(byte_count, fd->public.size - fd->public.position);

  while (byte_count > 0) {
    u32 left_in_buffer = SECTOR_SIZE - (fd->public.position % SECTOR_SIZE);
    u32 take = min(byte_count, left_in_buffer);

    memcpy(u8_data_out, fd->buffer + fd->public.position % SECTOR_SIZE, take);
    u8_data_out += take;
    fd->public.position += take;
    byte_count -= take;

    // See if we need to read more data
    if (left_in_buffer == take) {
      // Special handling for root directory
      if (fd->public.handle == ROOT_DIRECTORY_HANDLE) {
        ++fd->current_cluster;

        // read next sector
        if (!Disk_read_sectors(disk, fd->current_cluster, 1, fd->buffer)) {
          printf("FAT: read error!\r\n");
          break;
        }
      } else {
        // calculate next cluster & sector to read
        if (++fd->current_sector_in_cluster >=
            g_data->BS.boot_sector.sectors_per_cluster) {
          fd->current_sector_in_cluster = 0;
          fd->current_cluster = next_cluster(fd->current_cluster);
        }

        if (fd->current_cluster >= 0xFF8) {
          // Mark end of file
          fd->public.size = fd->public.position;
          break;
        }

        // read next sector
        if (!Disk_read_sectors(disk,
                               FAT_cluster_to_lba(fd->current_cluster) +
                                   fd->current_sector_in_cluster,
                               1, fd->buffer)) {
          printf("FAT: read error!\r\n");
          break;
        }
      }
    }
  }

  return u8_data_out - (u8 *)out_data;
}
bool FAT_read_entry(Disk *disk, FATFile far *file,
                    FATDirectoryEntry *out_data) {
  return FAT_read(disk, file, sizeof(FATDirectoryEntry), out_data) ==
         sizeof(FATDirectoryEntry);
}
void FAT_close(FATFile far *file) {
  if (file->handle == ROOT_DIRECTORY_HANDLE) {
    file->position = 0;
    g_data->root_directory.current_cluster =
        g_data->root_directory.first_cluster;
  } else {
    g_data->opened_files[file->handle].opened = false;
  }
}

FATFile far *FAT_open(Disk *disk, const char *path) {
  char name[MAX_PATH_SIZE];

  // ignore leading slash
  if (path[0] == '/')
    path++;

  FATFile far *current = &g_data->root_directory.public;

  while (*path) {
    // extract next file name from path
    bool isLast = false;
    const char *delim = strchr(path, '/');
    if (delim != NULL) {
      memcpy(name, path, delim - path);
      name[delim - path] = '\0';
      path = delim + 1;
    } else {
      unsigned len = strlen(path);
      memcpy(name, path, len);
      name[len] = '\0';
      path += len;
      isLast = true;
    }

    // find directory entry in current directory
    FATDirectoryEntry entry;
    if (find_file(disk, current, name, &entry)) {
      if (current != &g_data->root_directory.public) {
        FAT_close(current);
      }

      // check if directory
      if (!isLast && (entry.attributes & FAT_ATTRIBUTE_DIRECTORY) == 0) {
        printf("FAT: %s not a directory\r\n", name);
        return NULL;
      }

      // open new directory entry
      current = open_entry(disk, &entry);
    } else {
      if (current != &g_data->root_directory.public) {
        FAT_close(current);
      }

      printf("FAT: %s not found\r\n", name);
      return NULL;
    }
  }

  return current;
}
