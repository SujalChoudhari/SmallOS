#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include "string.h"
#include "file.h"

extern uint8_t __bss_start;
extern uint8_t __end;

// Converts a string to an integer (basic implementation)
int atoi(const char* str) {
    int num = 0;
    while (*str >= '0' && *str <= '9') {
        num = num * 10 + (*str - '0');
        str++;
    }
    return num;
}

// Simulate converting a directory path to a valid file name by replacing '/' with '_'
void convert_path_to_filename(char* path, char* filename) {
    int i = 0;
    while (*path) {
        if (*path == '/') {
            filename[i] = '_'; // Replace '/' with '_'
        } else {
            filename[i] = *path;
        }
        i++;
        path++;
    }
    filename[i] = '\0'; // Null-terminate the filename
}

// Global variable to keep track of the current working directory
char current_directory[256] = "home";

// Initialize default home directory with test files and subdirectories
void initialize_home_directory() {
    fs_create("one.txt");
    fs_create("two.txt");
    fs_create("three.txt"); // Simulate a file inside a subdirectory
    fs_create("four.txt");
}

// Command: pwd (print working directory)
void cmd_pwd() {
    printf("%s\n", current_directory);
}

// Command: ls (list files)
void cmd_ls() {
    fs_list(); // This lists the files in the "filesystem" (no native directory support)
}

// Command: cd (change directory)
void cmd_cd(const char* new_dir) {
    if (strcmp(new_dir, "..") == 0) {
        // Go to parent directory
        char* last_slash = strrchr(current_directory, '/');
        if (last_slash) {
            *last_slash = '\0'; // Truncate at last '/'
        } else {
            strcpy(current_directory, "home"); // Prevent navigating above "home"
        }
    } else {
        // Navigate to a subdirectory
        if (strlen(current_directory) + strlen(new_dir) + 1 < sizeof(current_directory)) {
            strcat(current_directory, "/");
            strcat(current_directory, new_dir);
        } else {
            printf("Path too long!\n");
        }
    }
}

// Command: mkdir (make directory - simulated)
void cmd_mkdir(const char* dir_name) {
    char dir_filename[256];
    convert_path_to_filename(dir_name, dir_filename);
    if (fs_create(dir_filename) == 0) {
        printf("Directory created: %s\n", dir_name);
    } else {
        printf("Failed to create directory: %s\n", dir_name);
    }
}

// Command: help (display available commands)
void cmd_help() {
    printf("\nAvailable commands:\n");
    printf("echo <text>           : Print text to the console\n");
    printf("exit                 : Exit the CLI\n");
    printf("add <num1> <num2>     : Add two numbers\n");
    printf("touch <file>         : Create a file\n");
    printf("write <file> <text>   : Write text to a file\n");
    printf("cat <file>           : Display the contents of a file\n");
    printf("ls                   : List files\n");
    printf("pwd                  : Show current directory\n");
    printf("cd <dir>             : Change directory\n");
    printf("mkdir <dir>          : Create a directory\n");
    printf("cls                  : Clear the screen\n");
    printf("help                 : Show this help message\n");
}

// Enhanced CLI function
void cli() {
    // Initialize file system and home directory with test files
    fs_init();
    initialize_home_directory();

    printf("\n====================\n");
    printf(" Welcome to SmallOS ");
    printf("\n====================\n");

    // Main command loop
    while (1) {
        // Updated prompt format with current directory
        printf("\n%s$ > ", current_directory); // CLI prompt showing current directory

        char input_buffer[256]; // Buffer to store user input
        memset(input_buffer, 0, sizeof(input_buffer)); // Clear buffer
        char command[64];       // For the command name
        char argument1[128];    // First argument (used for file names or directories)
        char argument2[128];    // Second argument (used for content or numbers)

        // Initialize argument buffers
        memset(command, 0, sizeof(command));
        memset(argument1, 0, sizeof(argument1));
        memset(argument2, 0, sizeof(argument2));

        // Read the full command input (command + 2 arguments max)
        scanf("%s %s %s", command, argument1, argument2); 

        // Handle different commands
        if (strcmp(command, "echo") == 0) {
            printf(">>> %s\n", argument1); // Echo the string back
        } else if (strcmp(command, "exit") == 0) {
            printf("Exiting CLI...\n");
            break; // Exit the loop
        } else if (strcmp(command, "add") == 0) {
            int x = atoi(argument1); // Convert first number
            int y = atoi(argument2); // Convert second number
            printf("Result: %d\n", x + y); // Print the result
        } else if (strcmp(command, "touch") == 0) {
            if (fs_create(argument1) == 0) {
                printf("File created: %s\n", argument1);
            } else {
                printf("Failed to create file: %s\n", argument1);
            }
        } else if (strcmp(command, "cls") == 0) {
            clrscr();
        } else if (strcmp(command, "cat") == 0) {
            File* file = fs_open(argument1);
            if (file) {
                char buffer[MAX_FILE_SIZE + 1];
                int bytes_read = fs_read(file, buffer, MAX_FILE_SIZE);
                if (bytes_read >= 0) {
                    buffer[bytes_read] = '\0';
                    printf("%s\n", buffer);
                } else {
                    printf("Error reading file: %s\n", argument1);
                }
                fs_close(file);
            } else {
                printf("File not found: %s\n", argument1);
            }
        } else if (strcmp(command, "write") == 0) {
            File* file = fs_open(argument1);
            if (file) {
                int bytes_written = fs_write(file, argument2, strlen(argument2));
                if (bytes_written >= 0) {
                    printf("Content written to file: %s\n", argument1);
                } else {
                    printf("Error writing to file: %s\n", argument1);
                }
                fs_close(file);
            } else {
                printf("Failed to open file: %s\n", argument1);
            }
        } else if (strcmp(command, "ls") == 0) {
            cmd_ls();
        } else if (strcmp(command, "pwd") == 0) {
            cmd_pwd();
        } else if (strcmp(command, "cd") == 0) {
            cmd_cd(argument1);
        } else if (strcmp(command, "mkdir") == 0) {
            cmd_mkdir(argument1);
        } else if (strcmp(command, "help") == 0) {
            cmd_help();
        } else {
            printf("Unknown command: %s\n", command); // Handle unknown commands
            printf("Type 'help' for a list of available commands.\n");
        }
    }
}

// Entry point of the program
void __attribute__((section(".entry"))) start(uint16_t bootDrive) {
    // Initialize BSS segment
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    clrscr(); // Clear screen

    cli(); // Start command line interface

    // Hang in an infinite loop after exiting CLI
    for (;;)
    {
        // Do nothing, just hang
    }
}
