#pragma once
#include <stdint.h>
#include <stdbool.h>

#define MAX_FILES 16
#define MAX_FILENAME_LENGTH 32
#define MAX_FILE_SIZE 4096

typedef struct
{
    char name[MAX_FILENAME_LENGTH];
    uint8_t data[MAX_FILE_SIZE];
    uint32_t size;
    bool is_open;
} File;

typedef struct
{
    File files[MAX_FILES];
    uint32_t file_count;
} FileSystem;

// Initialize the file system
void fs_init(void);

// Create a new file
int fs_create(const char *filename);

// Open a file
File *fs_open(const char *filename);

// Close a file
void fs_close(File *file);

// Read from a file
int fs_read(File *file, void *buffer, uint32_t size);

// Write to a file
int fs_write(File *file, const void *buffer, uint32_t size);

// Delete a file
int fs_delete(const char *filename);

// List all files
void fs_list(void);

