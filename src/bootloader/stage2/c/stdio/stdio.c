#include "../stdio/stdio.h"
#include "../external/x86.h"

void putc(char c)
{
    x86_Video_WriteCharTeletype(c, 0);
}

void puts(const char* str)
{
    while(*str)
    {
        putc(*str);
        str++;
    }

}

void putln(const char* str)
{
    puts(str);
    puts("\r\n");
}
