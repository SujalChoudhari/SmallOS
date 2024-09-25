#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include "string.h"

extern uint8_t __bss_start;
extern uint8_t __end;
int atoi(const char* str) {
    int num = 0;
    while (*str >= '0' && *str <= '9') {
        num = num * 10 + (*str - '0');
        str++;
    }
    return num;
}

// CLI function
void cli()
{
    // Main command loop
    while (1)
    {
        // Updated prompt format
        printf("$ > "); // CLI prompt

        char input_buffer[256]; // Buffer to store user input
        // Initialize the input buffer to empty
        memset(input_buffer, 0, sizeof(input_buffer));

        char values[256];
        // Read the full command input
        scanf("%s %s", input_buffer, values); // Read the command

        // Handle different commands
        if (startswith(input_buffer, "echo")) {
            // Prepare to echo the rest of the input
            const char* echo_text = input_buffer + 4; // Skip "echo "
            printf("---> %s\n", values); // Echo the string back
        }
        else if (strcmp(input_buffer, "exit") == 0) {
            printf("Exiting CLI...\n");
            break; // Exit the loop
        }
        else if (startswith(input_buffer, "add ")) {
            // Prepare to read two numbers
            int x = 0;
            int y = 0;

            // Use a temporary buffer to hold the rest of the input
            char temp_buffer[256];
            scanf("%s %s", temp_buffer, input_buffer + 4); // Read numbers directly
            
            x = atoi(temp_buffer); // Convert first number
            y = atoi(input_buffer + 4); // Convert second number
            
            printf("Result: %d\n", x + y); // Print the result
        }
        else {
            printf("Unknown command: %s\n", input_buffer); // Handle unknown commands
        }
    }
}


// Entry point of the program
void __attribute__((section(".entry"))) start(uint16_t bootDrive)
{
    // Initialize BSS segment
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    clrscr(); // Clear screen

    printf("Welcome to SmallOS\n");

    cli(); // Start command line interface

    // Hang in an infinite loop after exiting CLI
    for (;;)
    {
        // Do nothing, just hang
    }
}
