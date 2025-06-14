/**
 * flash.h
 * Copyright (c) 2025 MVK
 * 09-May-2025
 *
 */

#ifndef _STORE_FLASH_H_
#define _STORE_FLASH_H_

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include <hardware/flash.h>
#include <hardware/sync.h>

// PICO_FLASH_SIZE_BYTES #The total size of the RP2040 flash, in bytes = 2097152 Bytes (2MB)
// FLASH_SECTOR_SIZE #The size of one sector, in bytes(the minimum amount you can erase) = 4096 Bytes
// FLASH_PAGE_SIZE #The size of one page, in bytes(the mimimum amount you can write) = 256 Bytes

typedef struct
{
	char ssid[36];
	char pwd[64];
	int status;
	char padding[256 - (36 + 64 + 4)]; // total 256 bytes "FLASH_PAGE_SIZE" // "char" is allocated in multiples of four
} data_s;

int flash_erase(void);
int flash_write(void *ptr, int size);
int flash_read(void *ptr, int size);

int test_flash(void);

#endif // _STORE_FLASH_H_
