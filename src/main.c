/**
 * client.c
 * Copyright (c) 2025 MVK
 * 09-May-2025
 *
 */

#include "../http_client/http_client.h"
#include "./LCD_Test.h"
#include "../lib/LCD/LCD_1in3.h"
#include "../ble/ble.h"
#include "../tiny-json/tiny-json.h"
#include "../keys/check_keys.h"

#include "pico/multicore.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "string.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/http_client.h"
#include "lwip/timeouts.h"

UWORD *BlackImage;

extern wifi_setting_t wifi_setting;
extern volatile int is_connected;

#include "btstack.h"
#include "pico/btstack_cyw43.h"

extern int le_notification_enabled;
extern hci_con_handle_t con_handle;
extern wifi_setting_t wifi_setting;
extern volatile int link_status;
extern volatile int is_connected;

extern void wifi_task(void);
extern void device_task(void);

int main(void)
{
	stdio_init_all();

	int retval = 0;

	if (retval = DEV_Module_Init())
	{
		printf("Error: DEV_Module_Init() returned %d\r\n", retval);
		return -1;
	}

	LCD_1IN3_Init(HORIZONTAL);
	LCD_1IN3_Clear(WHITE);

	UDOUBLE Imagesize = LCD_1IN3_HEIGHT * LCD_1IN3_WIDTH * 2;

	if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL)
	{
		printf("Failed to apply for black memory...\r\n");
		exit(0);
	}

	Paint_NewImage((UBYTE *)BlackImage, LCD_1IN3.WIDTH, LCD_1IN3.HEIGHT, 0, WHITE);
	Paint_Clear(WHITE);
	Paint_SetScale(65);
	Paint_SetRotate(ROTATE_0);
	Paint_Clear(WHITE);
	set_brightness(10);

	check_keys_init();

	multicore_reset_core1();
	multicore_launch_core1(check_keys); // and also start http client requests, this should be renamed.

	start_ble();

	while (true)
	{
		wifi_task();
		device_task();

		if (le_notification_enabled)
		{
			att_server_request_can_send_now_event(con_handle);
		}

		sleep_us(10);
	}

	free(BlackImage);
	BlackImage = NULL;

	return 0;
}
