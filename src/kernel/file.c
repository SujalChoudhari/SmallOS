
// file.c
#include "file.h"
#include "stdio.h" // For printf function
#include "string.h"
#include "memory.h"
#define NULL 0
static FileSystem fs;

void fs_init(void) {
    fs.file_count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        fs.files[i].is_open = false;
    }
}

int fs_create(const char* filename) {
    if (fs.file_count >= MAX_FILES) {
        return -1; // File system is full
    }

    for (uint32_t i = 0; i < fs.file_count; i++) {
        if (strcmp(fs.files[i].name, filename) == 0) {
            return -2; // File already exists
        }
    }

    strcpy(fs.files[fs.file_count].name, filename);
    fs.files[fs.file_count].size = 0;
    fs.files[fs.file_count].is_open = false;
    fs.file_count++;

    return 0; // Success
}

File* fs_open(const char* filename) {
    for (uint32_t i = 0; i < fs.file_count; i++) {
        if (strcmp(fs.files[i].name, filename) == 0) {
            if (!fs.files[i].is_open) {
                fs.files[i].is_open = true;
                return &fs.files[i];
            } else {
                return NULL; // File is already open
            }
        }
    }
    return NULL; // File not found
}

void fs_close(File* file) {
    if (file) {
        file->is_open = false;
    }
}

int fs_read(File* file, void* buffer, uint32_t size) {
    if (!file || !file->is_open) {
        return -1; // Invalid file or file not open
    }

    uint32_t read_size = (size < file->size) ? size : file->size;
    memcpy(buffer, file->data, read_size);
    return read_size;
}

int fs_write(File* file, const void* buffer, uint32_t size) {
    if (!file || !file->is_open) {
        return -1; // Invalid file or file not open
    }

    if (size > MAX_FILE_SIZE) {
        size = MAX_FILE_SIZE;
    }

    memcpy(file->data, buffer, size);
    file->size = size;
    return size;
}

int fs_delete(const char* filename) {
    for (uint32_t i = 0; i < fs.file_count; i++) {
        if (strcmp(fs.files[i].name, filename) == 0) {
            if (fs.files[i].is_open) {
                return -1; // Can't delete an open file
            }

            // Move the last file to this position
            if (i < fs.file_count - 1) {
                fs.files[i] = fs.files[fs.file_count - 1];
            }

            fs.file_count--;
            return 0; // Success
        }
    }
    return -2; // File not found
}

void fs_list(void) {
    printf("Files in the system:\n");
    for (uint32_t i = 0; i < fs.file_count; i++) {
        printf("%s (size: %d bytes)\n", fs.files[i].name, fs.files[i].size);
    }
}
