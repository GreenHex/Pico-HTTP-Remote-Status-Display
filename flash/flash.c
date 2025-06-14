/**
 * flash.c
 * Copyright (c) 2025 MVK
 * 09-May-2025
 *
 * From: https://kevinboone.me/picoflash.html
 *
 */

#include "flash.h"

// FLASH_SECTOR_SIZE = 4096 bytes
// FLASH_PAGE_SIZE = 256 bytes

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)					  // (1790 * 1024)
const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET); //++ Pointer pointing at the Flash Address Location

int flash_erase(void)
{
	uint32_t ints = save_and_disable_interrupts();

	flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

	restore_interrupts_from_disabled(ints);

	return 0;
}

int flash_write(void *ptr, int size) // "size" is multiples of FLASH_PAGE_SIZE
{
	if (size % FLASH_PAGE_SIZE)
	{
		printf("Error: 'size' is not a multiple of FLASH_PAGE_SIZE, cannot write to flash.\n");
		return -1;
	}

	uint32_t ints = save_and_disable_interrupts();

	flash_erase();

	flash_range_program(FLASH_TARGET_OFFSET, (uint8_t *)ptr, size);

	restore_interrupts_from_disabled(ints);

	fflush(stdout);

	return 0;
}

int flash_read(void *ptr, int size) // "size" is multiples of FLASH_PAGE_SIZE
{
	if (size % FLASH_PAGE_SIZE)
	{
		printf("Error: 'size' is not a multiple of FLASH_PAGE_SIZE, cannot read from flash.\n");
		return -1;
	}

	char *p = (char *)flash_target_contents;

	memcpy(ptr, p, size);

	return 0;
}
