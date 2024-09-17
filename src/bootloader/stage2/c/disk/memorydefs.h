#pragma once

// 0x00000000 - 0x000003ff - inteupt vector tables
// 0x00000400 - 0x000004ff - bios data area
//
#define MEMORY_MIN 0x00000500
#define MEMORY_MAX 0x00080000

// 0x00000500 - 0x00010500 - FAT driver
#define MEMORY_FAT_ADDR ((void far *)0x00500000)
#define MEMORY_FAT_SIZE 0x00010500l

// 0x00020000 - 0x00030000 - stage 2
// 0x00030000 - 0x00080000 - free

// 0x00080000 - 0x0009ffff - Extended BIOS data area
// 0x000a0000 - 0x000c7fff - video
// 0x000c8000 - 0x000fffff - bios
