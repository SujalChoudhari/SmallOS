#include "stdtypes/stdint.h"
#include "stdio/stdio.h"
#include "stdtypes/bool.h"

void _cdecl cstart_(u16 bootDrive)
{
    putln("Hello world from C!");
    while(true) ;
}