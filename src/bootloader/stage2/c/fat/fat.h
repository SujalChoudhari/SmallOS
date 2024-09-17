#pragma once
#include "../disk/disk.h"
#include "../types/bool.h"
#include "../types/int.h"
// // Structure representing the FAT boot sector
// typedef struct
// {
//     u8 boot_jump_instruction[3]; // Jump instruction to the boot code
//     u8 oem_identifier[8];        // OEM identifier
//     u16 bytes_per_sector;        // Number of bytes per sector
//     u8 sectors_per_cluster;      // Number of sectors per cluster
//     u16 reserved_sectors;        // Number of reserved sectors
//     u8 fat_count;                // Number of FATs
//     u16 dir_entry_count;         // Number of root directory entries
//     u16 total_sectors;           // Total number of sectors (or 0 if using
//     large_sector_count) u8 media_descriptor_type;    // Media descriptor type
//     u16 sectors_per_fat;         // Number of sectors per FAT
//     u16 sectors_per_track;       // Number of sectors per track
//     u16 heads;                   // Number of heads (sides)
//     u32 hidden_sectors;          // Number of hidden sectors
//     u32 large_sector_count;      // Total number of sectors (for FAT32 or if
//     total_sectors == 0)

//     // Extended boot record (used in FAT12/16/32)
//     u8 drive_number;     // Drive number (usually 0x80 for the first hard
//     disk) u8 reserved;         // Reserved byte u8 signature;        //
//     Signature (should be 0x29) u32 volume_id;       // Volume serial number
//     u8 volume_label[11]; // Volume label (11 bytes, padded with spaces)
//     u8 system_id[8];     // File system type (e.g., "FAT12", "FAT16")

//     // ... we don't care about code ...
// } __attribute__((packed)) boot_sector;

#pragma pack(push, 1)
// Structure representing a directory entry in the FAT file system
typedef struct {
  u8 name[11];   // File name (8.3 format: 8 characters name, 3 characters
                 // extension)
  u8 attributes; // File attributes (e.g., read-only, hidden, system)
  u8 reserved;   // Reserved field
  u8 created_time_tenths; // Tenths of a second when the file was created
  u16 created_time;       // Creation time
  u16 created_date;       // Creation date
  u16 accessed_date;      // Last access date
  u16 first_cluster_high; // High 16 bits of the first cluster number
  u16 modified_time;      // Last modified time
  u16 modified_date;      // Last modified date
  u16 first_cluster_low;  // Low 16 bits of the first cluster number
  u32 size;               // File size in bytes
} FATDirectoryEntry;

#pragma pack(pop)

typedef struct {
  int handle;
  bool is_directory;
  i32 position;
  u32 size;
} FATFile;

enum FATAttributes {
  FAT_ATTRIBUTE_READ_ONLY = 0x01,
  FAT_ATTRIBUTE_HIDDEN = 0x02,
  FAT_ATTRIBUTE_SYSTEM = 0x04,
  FAT_ATTRIBUTE_VOLUME_ID = 0x08,
  FAT_ATTRIBUTE_DIRECTORY = 0x10,
  FAT_ATTRIBUTE_ARCHIVE = 0x20,
  FAT_ATTRIBUTE_LFN = FAT_ATTRIBUTE_READ_ONLY | FAT_ATTRIBUTE_HIDDEN |
                      FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID |
                      FAT_ATTRIBUTE_DIRECTORY | FAT_ATTRIBUTE_ARCHIVE
};

bool FATDirectoryEntry_init(Disk *disk);
FATFile FAT_open(Disk *disk, const char *path);
u32 FAT_read(Disk *disk, FATFile far *file, u32 byte_count, void *out_data);
bool FAT_read_entry(Disk *disk, FATFile far *file, FATDirectoryEntry *out_data);
void FAT_close(FATFile far* file);
