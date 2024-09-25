#include "stdio.h"
#include "x86.h"

#include <stdarg.h>
#include <stdbool.h>

const unsigned SCREEN_WIDTH = 80;
const unsigned SCREEN_HEIGHT = 25;
const uint8_t DEFAULT_COLOR = 0x7;

bool shift_pressed = false;
bool caps_lock = false;

// Add these global variables
#define UNGETCH_BUF_SIZE 16
char ungetch_buf[UNGETCH_BUF_SIZE];
int ungetch_index = 0;

uint8_t *g_ScreenBuffer = (uint8_t *)0xB8000;
int g_ScreenX = 0, g_ScreenY = 0;

void putchr(int x, int y, char c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)] = c;
}

void putcolor(int x, int y, uint8_t color)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = color;
}

char getchr(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)];
}

uint8_t getcolor(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1];
}

void setcursor(int x, int y)
{
    int pos = y * SCREEN_WIDTH + x;

    x86_outb(0x3D4, 0x0F);
    x86_outb(0x3D5, (uint8_t)(pos & 0xFF));
    x86_outb(0x3D4, 0x0E);
    x86_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void clrscr()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y, '\0');
            putcolor(x, y, DEFAULT_COLOR);
        }

    g_ScreenX = 0;
    g_ScreenY = 0;
    setcursor(g_ScreenX, g_ScreenY);
}

void scrollback(int lines)
{
    for (int y = lines; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y - lines, getchr(x, y));
            putcolor(x, y - lines, getcolor(x, y));
        }

    for (int y = SCREEN_HEIGHT - lines; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y, '\0');
            putcolor(x, y, DEFAULT_COLOR);
        }

    g_ScreenY -= lines;
}

void putc(char c)
{
    switch (c)
    {
    case '\n':
        g_ScreenX = 0;
        g_ScreenY++;
        break;

    case '\t':
        for (int i = 0; i < 4 - (g_ScreenX % 4); i++)
            putc(' ');
        break;

    case '\r':
        g_ScreenX = 0;
        break;

    default:
        putchr(g_ScreenX, g_ScreenY, c);
        g_ScreenX++;
        break;
    }

    if (g_ScreenX >= SCREEN_WIDTH)
    {
        g_ScreenY++;
        g_ScreenX = 0;
    }
    if (g_ScreenY >= SCREEN_HEIGHT)
        scrollback(1);

    setcursor(g_ScreenX, g_ScreenY);
}

void puts(const char *str)
{
    while (*str)
    {
        putc(*str);
        str++;
    }
}

const char g_HexChars[] = "0123456789abcdef";

void printf_unsigned(unsigned long long number, int radix)
{
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do
    {
        unsigned long long rem = number % radix;
        number /= radix;
        buffer[pos++] = g_HexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        putc(buffer[pos]);
}

void printf_signed(long long number, int radix)
{
    if (number < 0)
    {
        putc('-');
        printf_unsigned(-number, radix);
    }
    else
        printf_unsigned(number, radix);
}

#define PRINTF_STATE_NORMAL 0
#define PRINTF_STATE_LENGTH 1
#define PRINTF_STATE_LENGTH_SHORT 2
#define PRINTF_STATE_LENGTH_LONG 3
#define PRINTF_STATE_SPEC 4

#define PRINTF_LENGTH_DEFAULT 0
#define PRINTF_LENGTH_SHORT_SHORT 1
#define PRINTF_LENGTH_SHORT 2
#define PRINTF_LENGTH_LONG 3
#define PRINTF_LENGTH_LONG_LONG 4

void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;
    bool number = false;

    while (*fmt)
    {
        switch (state)
        {
        case PRINTF_STATE_NORMAL:
            switch (*fmt)
            {
            case '%':
                state = PRINTF_STATE_LENGTH;
                break;
            default:
                putc(*fmt);
                break;
            }
            break;

        case PRINTF_STATE_LENGTH:
            switch (*fmt)
            {
            case 'h':
                length = PRINTF_LENGTH_SHORT;
                state = PRINTF_STATE_LENGTH_SHORT;
                break;
            case 'l':
                length = PRINTF_LENGTH_LONG;
                state = PRINTF_STATE_LENGTH_LONG;
                break;
            default:
                goto PRINTF_STATE_SPEC_;
            }
            break;

        case PRINTF_STATE_LENGTH_SHORT:
            if (*fmt == 'h')
            {
                length = PRINTF_LENGTH_SHORT_SHORT;
                state = PRINTF_STATE_SPEC;
            }
            else
                goto PRINTF_STATE_SPEC_;
            break;

        case PRINTF_STATE_LENGTH_LONG:
            if (*fmt == 'l')
            {
                length = PRINTF_LENGTH_LONG_LONG;
                state = PRINTF_STATE_SPEC;
            }
            else
                goto PRINTF_STATE_SPEC_;
            break;

        case PRINTF_STATE_SPEC:
        PRINTF_STATE_SPEC_:
            switch (*fmt)
            {
            case 'c':
                putc((char)va_arg(args, int));
                break;

            case 's':
                puts(va_arg(args, const char *));
                break;

            case '%':
                putc('%');
                break;

            case 'd':
            case 'i':
                radix = 10;
                sign = true;
                number = true;
                break;

            case 'u':
                radix = 10;
                sign = false;
                number = true;
                break;

            case 'X':
            case 'x':
            case 'p':
                radix = 16;
                sign = false;
                number = true;
                break;

            case 'o':
                radix = 8;
                sign = false;
                number = true;
                break;

            // ignore invalid spec
            default:
                break;
            }

            if (number)
            {
                if (sign)
                {
                    switch (length)
                    {
                    case PRINTF_LENGTH_SHORT_SHORT:
                    case PRINTF_LENGTH_SHORT:
                    case PRINTF_LENGTH_DEFAULT:
                        printf_signed(va_arg(args, int), radix);
                        break;

                    case PRINTF_LENGTH_LONG:
                        printf_signed(va_arg(args, long), radix);
                        break;

                    case PRINTF_LENGTH_LONG_LONG:
                        printf_signed(va_arg(args, long long), radix);
                        break;
                    }
                }
                else
                {
                    switch (length)
                    {
                    case PRINTF_LENGTH_SHORT_SHORT:
                    case PRINTF_LENGTH_SHORT:
                    case PRINTF_LENGTH_DEFAULT:
                        printf_unsigned(va_arg(args, unsigned int), radix);
                        break;

                    case PRINTF_LENGTH_LONG:
                        printf_unsigned(va_arg(args, unsigned long), radix);
                        break;

                    case PRINTF_LENGTH_LONG_LONG:
                        printf_unsigned(va_arg(args, unsigned long long), radix);
                        break;
                    }
                }
            }

            // reset state
            state = PRINTF_STATE_NORMAL;
            length = PRINTF_LENGTH_DEFAULT;
            radix = 10;
            sign = false;
            break;
        }

        fmt++;
    }

    va_end(args);
}

void print_buffer(const char *msg, const void *buffer, uint32_t count)
{
    const uint8_t *u8Buffer = (const uint8_t *)buffer;

    puts(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        putc(g_HexChars[u8Buffer[i] >> 4]);
        putc(g_HexChars[u8Buffer[i] & 0xF]);
    }
    puts("\n");
}
char getch()
{
    static bool key_is_released = true;

    if (ungetch_index > 0)
    {
        return ungetch_buf[--ungetch_index];
    }

    while (1)
    {
        uint8_t scancode = x86_inb(0x60);  // Get scancode from keyboard buffer

        if (scancode & 0x80)  // Key release (high bit is set)
        {
            key_is_released = true;
            continue;
        }
        else if (key_is_released)  // Only process if the key was released previously
        {
            key_is_released = false;  // Mark key as pressed (waiting for release)

            char c = scancode_to_ascii(scancode);  // Convert scancode to ASCII
            if (c)
            {
                putc(c);  // Echo the character
                return c; // Return the character
            }
        }
    }
}


void ungetch(char c)
{
    if (ungetch_index < UNGETCH_BUF_SIZE)
    {
        ungetch_buf[ungetch_index++] = c;
    }
}
char scancode_to_ascii(uint8_t scancode)
{
    // Mapping basic keys to ASCII
    if (scancode >= 0x02 && scancode <= 0x0D)
    {
        return "1234567890-="[scancode - 0x02];
    }
    if (scancode >= 0x10 && scancode <= 0x1B)
    {
        return "qwertyuiop[]"[scancode - 0x10];
    }
    if (scancode >= 0x1E && scancode <= 0x28)
    {
        return "asdfghjkl;'"[scancode - 0x1E];
    }
    if (scancode >= 0x2C && scancode <= 0x35)
    {
        return "zxcvbnm,./"[scancode - 0x2C];
    }

    // Space key
    if (scancode == 0x39)
    {
        return ' ';
    }

    // Enter key (Scancode: 0x1C)
    if (scancode == 0x1C)
    {
        return '\n';
    }

    // Backspace key (Scancode: 0x0E)
    if (scancode == 0x0E)
    {
        return '\b';
    }

    // Unknown scancode
    return 0;
}


int scanf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int items_matched = 0;
    char input_buffer[256];  // Temporary buffer to store the input line
    int buffer_index = 0;

    // Read input one character at a time until newline is encountered
    char c;
    do
    {
        c = getch();  // Get character input

        if (c == '\b' && buffer_index > 0)  // Handle backspace
        {
            buffer_index--;  // Remove last character from buffer
            g_ScreenX--;     // Move cursor back
            putc(' ');       // Erase character on screen
            g_ScreenX--;     // Move cursor back again
            setcursor(g_ScreenX, g_ScreenY);
        }
        else if (c != '\b' && c != '\n')  // Ignore newline here, we'll process it later
        {
            input_buffer[buffer_index++] = c;  // Add character to buffer
        }

    } while (c != '\n' && buffer_index < sizeof(input_buffer) - 1);

    input_buffer[buffer_index] = '\0';  // Null-terminate the string

    // Now process the format string and match inputs
    const char *input_ptr = input_buffer;
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 'd':  // Integer input
            {
                int *p = va_arg(args, int *);
                int value = 0;
                int sign = 1;

                // Check for negative sign
                if (*input_ptr == '-')
                {
                    sign = -1;
                    input_ptr++;
                }

                // Read digits
                while (*input_ptr >= '0' && *input_ptr <= '9')
                {
                    value = value * 10 + (*input_ptr - '0');
                    input_ptr++;
                }

                *p = value * sign;
                items_matched++;
                break;
            }

            case 's':  // String input
            {
                char *p = va_arg(args, char *);
                while (*input_ptr != ' ' && *input_ptr != '\0')
                {
                    *p++ = *input_ptr++;
                }
                *p = '\0';  // Null-terminate the string
                items_matched++;
                break;
            }

            // Add more format specifiers as needed
            default:
                break;
            }
        }
        else
        {
            // Match literal characters in format string
            if (*fmt != *input_ptr)
            {
                break;  // Mismatch
            }
            input_ptr++;
        }
        fmt++;
    }

    va_end(args);
    return items_matched;
}
