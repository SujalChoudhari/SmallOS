#pragma once

#include "../types/types.h"

/**
 * @brief Represents a disk drive
 */
typedef struct {
  u8 id;           /**< Disk identifier */
  u16 cylinders;   /**< Number of cylinders */
  u16 sectors;     /**< Number of sectors per track */
  u16 heads;       /**< Number of heads */
} Disk;

/**
 * @brief Initialize a disk
 * @param disk Pointer to the Disk structure to initialize
 * @param driveNumber The drive number to initialize
 * @return true if initialization was successful, false otherwise
 */
bool Disk_init(Disk *disk, u8 driveNumber);

/**
 * @brief Read sectors from the disk
 * @param disk Pointer to the Disk structure
 * @param lba Logical Block Address to start reading from
 * @param sectors Number of sectors to read
 * @param out_data Pointer to the buffer where read data will be stored
 * @return true if read was successful, false otherwise
 */
bool Disk_read_sectors(Disk *disk, u32 lba, u8 sectors, void far *out_data);

/**
 * @brief Convert Logical Block Address (LBA) to Cylinder-Head-Sector (CHS)
 * @param disk Pointer to the Disk structure
 * @param lba Logical Block Address to convert
 * @param out_cylinder Pointer to store the calculated cylinder
 * @param out_sector Pointer to store the calculated sector
 * @param out_head Pointer to store the calculated head
 */
void Disk_lba_to_chs(Disk *disk, u32 lba, u16 *out_cylinder, u16 *out_sector, u16* out_head);
