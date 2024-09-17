#pragma once
#include "../types/bool.h"
#include "../types/int.h"
#include "disk.h"

#pragma pack(push, 1)
/**
 * Structure representing a directory entry in the FAT file system
 */
typedef struct {
  u8 name[11];
  u8 attributes;
  u8 reserved;
  u8 created_time_tenths;
  u16 created_time;
  u16 created_date;
  u16 accessed_date;
  u16 first_cluster_high;
  u16 modified_time;
  u16 modified_date;
  u16 first_cluster_low;
  u32 size;
} FATDirectoryEntry;
#pragma pack(pop)

/**
 * Structure representing a file in the FAT file system
 */
typedef struct {
  int handle;
  bool is_directory;
  i32 position;
  u32 size;
} FATFile;

/**
 * Enumeration of FAT file attributes
 */
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

/**
 * Initialize a FAT directory entry
 * @param disk Pointer to the disk
 * @return True if initialization is successful, false otherwise
 */
bool FATDirectoryEntry_init(Disk *disk);

/**
 * Open a file or directory in the FAT file system
 * @param disk Pointer to the disk
 * @param path Path to the file or directory
 * @return Pointer to the opened FATFile structure
 */
FATFile far *FAT_open(Disk *disk, const char *path);

/**
 * Read data from a file in the FAT file system
 * @param disk Pointer to the disk
 * @param file Pointer to the FATFile structure
 * @param byte_count Number of bytes to read
 * @param out_data Pointer to the buffer to store read data
 * @return Number of bytes actually read
 */
u32 FAT_read(Disk *disk, FATFile far *file, u32 byte_count, void *out_data);

/**
 * Read a directory entry from a directory in the FAT file system
 * @param disk Pointer to the disk
 * @param file Pointer to the FATFile structure representing the directory
 * @param out_data Pointer to the FATDirectoryEntry structure to store the read
 * entry
 * @return True if a directory entry was successfully read, false otherwise
 */
bool FAT_read_entry(Disk *disk, FATFile far *file, FATDirectoryEntry *out_data);

/**
 * Close a file or directory in the FAT file system
 * @param file Pointer to the FATFile structure to close
 */
void FAT_close(FATFile far *file);
