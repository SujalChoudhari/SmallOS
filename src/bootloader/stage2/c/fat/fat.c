//  TODO: NOW
// #include "fat.h"

// // Define a boolean type for compatibility with C
// typedef uint8_t bool;
// #define true 1
// #define false 0



// // Global variables
// boot_sector_t g_boot_sector;                // Global variable for the boot sector
// uint8_t *g_fat = NULL;                      // Global variable for the FAT
// directory_entry_t *g_root_directory = NULL; // Global variable for the root directory entries
// uint32_t g_root_directory_end;              // End LBA of the root directory

// // Reads the boot sector from the disk image into the global `g_boot_sector`
// bool read_boot_sector(FILE *disk)
// {
//     return fread(&g_boot_sector, sizeof(g_boot_sector), 1, disk) > 0;
// }

// // Reads a specified number of sectors from the disk image starting at LBA into a buffer
// bool read_sectors(FILE *disk, uint32_t lba, uint32_t count, void *buffer_out)
// {
//     bool ok = true;
//     ok = ok && (fseek(disk, lba * g_boot_sector.bytes_per_sector, SEEK_SET) == 0);
//     ok = ok && (fread(buffer_out, g_boot_sector.bytes_per_sector, count, disk) == count);
//     return ok;
// }

// // Reads the FAT from the disk image into the global `g_fat`
// bool read_fat(FILE *disk)
// {
//     g_fat = (uint8_t *)malloc(g_boot_sector.sectors_per_fat * g_boot_sector.bytes_per_sector);
//     return read_sectors(disk, g_boot_sector.reserved_sectors, g_boot_sector.sectors_per_fat, g_fat);
// }

// // Reads the root directory entries from the disk image into the global `g_root_directory`
// bool read_root_directory(FILE *disk)
// {
//     uint32_t lba = g_boot_sector.reserved_sectors + g_boot_sector.sectors_per_fat * g_boot_sector.fat_count;
//     uint32_t size = sizeof(directory_entry_t) * g_boot_sector.dir_entry_count;
//     uint32_t sectors = (size / g_boot_sector.bytes_per_sector);
//     if (size % g_boot_sector.bytes_per_sector > 0)
//         sectors++;

//     g_root_directory_end = lba + sectors;
//     g_root_directory = (directory_entry_t *)malloc(sectors * g_boot_sector.bytes_per_sector);
//     return read_sectors(disk, lba, sectors, g_root_directory);
// }

// // Finds a file entry in the root directory by name
// directory_entry_t *find_file(const char *name)
// {
//     for (uint32_t i = 0; i < g_boot_sector.dir_entry_count; i++)
//     {
//         if (memcmp(name, g_root_directory[i].name, 11) == 0)
//             return &g_root_directory[i];
//     }
//     return NULL;
// }

// // Reads the contents of a file specified by the `directory_entry_t` into a buffer
// bool read_file(directory_entry_t *file_entry, FILE *disk, uint8_t *output_buffer)
// {
//     bool ok = true;
//     uint16_t current_cluster = file_entry->first_cluster_low;

//     do
//     {
//         uint32_t lba = g_root_directory_end + (current_cluster - 2) * g_boot_sector.sectors_per_cluster;
//         ok = ok && read_sectors(disk, lba, g_boot_sector.sectors_per_cluster, output_buffer);
//         output_buffer += g_boot_sector.sectors_per_cluster * g_boot_sector.bytes_per_sector;

//         uint32_t fat_index = current_cluster * 3 / 2;
//         if (current_cluster % 2 == 0)
//             current_cluster = (*(uint16_t *)(g_fat + fat_index)) & 0x0FFF;
//         else
//             current_cluster = (*(uint16_t *)(g_fat + fat_index)) >> 4;

//     } while (ok && current_cluster < 0x0FF8);

//     return ok;
// }

// // Main function
// int main(int argc, char **argv)
// {
//     if (argc < 3)
//     {
//         printf("Syntax: %s <disk image> <file name>\n", argv[0]);
//         return -1;
//     }

//     FILE *disk = fopen(argv[1], "rb");
//     if (!disk)
//     {
//         fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
//         return -1;
//     }

//     // Read the boot sector
//     if (!read_boot_sector(disk))
//     {
//         fprintf(stderr, "Could not read boot sector!\n");
//         return -2;
//     }

//     // Read the FAT
//     if (!read_fat(disk))
//     {
//         fprintf(stderr, "Could not read FAT!\n");
//         free(g_fat);
//         return -3;
//     }

//     // Read the root directory
//     if (!read_root_directory(disk))
//     {
//         fprintf(stderr, "Could not read root directory!\n");
//         free(g_fat);
//         free(g_root_directory);
//         return -4;
//     }

//     // Find the file in the root directory
//     directory_entry_t *file_entry = find_file(argv[2]);
//     if (!file_entry)
//     {
//         fprintf(stderr, "Could not find file %s!\n", argv[2]);
//         free(g_fat);
//         free(g_root_directory);
//         return -5;
//     }

//     // Allocate buffer and read the file
//     uint8_t *buffer = (uint8_t *)malloc(file_entry->size + g_boot_sector.bytes_per_sector);
//     if (!read_file(file_entry, disk, buffer))
//     {
//         fprintf(stderr, "Could not read file %s!\n", argv[2]);
//         free(g_fat);
//         free(g_root_directory);
//         free(buffer);
//         return -6;
//     }

//     // Print the file content
//     for (size_t i = 0; i < file_entry->size; i++)
//     {
//         if (isprint(buffer[i]))
//             fputc(buffer[i], stdout); // Print printable characters
//         else
//             printf("<%02x>", buffer[i]); // Print non-printable characters in hex
//     }
//     printf("\n");

//     // Clean up and exit
//     free(buffer);
//     free(g_fat);
//     free(g_root_directory);
//     return 0;
// }
