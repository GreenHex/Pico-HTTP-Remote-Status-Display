/**
 * multicore.c
 * Copyright (c) 2025 MVK
 * 23-Apr-2025
 *
 */

#include "check_keys.h"
#include "../http_client/http_client.h"
#include "../ble/ble.h"
#include "pico/multicore.h"
#include "../lib/Infrared/Infrared.h"
#include "../lib/Config/DEV_Config.h"

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
#include "../tcp_client/tcp_client.h"
#include "../tiny-json/json_proc.h"
#include "hardware/watchdog.h"

extern uint total_requests_sent;
extern uint total_resolved_requests;
extern uint continuous_unresolved_request_count;
extern const uint UNRESOLVED_REQUEST_COUNT_THRESHOLD;
extern uint total_lwip_resets;

// extern UWORD *BlackImage;
extern wifi_setting_t wifi_setting;
extern volatile int is_connected;
// extern const absolute_time_t REQUEST_DELAY;

extern void process_event(device_event_t event);

uint8_t keyA = 15;
uint8_t keyB = 17;
uint8_t keyX = 19;
uint8_t keyY = 21;

uint8_t up = 2;
uint8_t dowm = 18;
uint8_t left = 16;
uint8_t right = 20;
uint8_t ctrl = 3;

void check_keys_init(void)
{
	SET_Infrared_PIN(keyA);
	SET_Infrared_PIN(keyB);
	SET_Infrared_PIN(keyX);
	SET_Infrared_PIN(keyY);

	SET_Infrared_PIN(up);
	SET_Infrared_PIN(dowm);
	SET_Infrared_PIN(left);
	SET_Infrared_PIN(right);
	SET_Infrared_PIN(ctrl);
}

// #define AIRCR_Register (*((volatile uint32_t *)(PPB_BASE + 0x0ED0C)))

extern char HEADING[];
const char STR_RESET_HEADING[] = "CW43 RESET";

// #ifndef USE_HTTP
// #define USE_TCP
// #endif // USE_HTTP

void check_keys(void)
{
	static volatile bool display_toggle = true;
	const int display_dim[] = {0, 10, 25, 50, 100};
	static volatile int display_dim_idx = (sizeof(display_dim) / sizeof(display_dim[0])) - 1;
	const int display_dim_max = (sizeof(display_dim) / sizeof(display_dim[0])) - 1;

	absolute_time_t old_time = -REQUEST_DELAY;
	absolute_time_t new_time = 0;

	httpc_connection_t settings = {
		.use_proxy = 0,
		.result_fn = result_fn,
		.headers_done_fn = headers_done_fn};

	httpc_state_t *connection = NULL;

	while (1)
	{
		if (!DEV_Digital_Read(keyA))
		{
			display_dim_idx = (display_toggle = !display_toggle) ? display_dim_max : 0;
			set_brightness(display_dim[display_dim_idx]);
			sleep_ms(500);
		}
		else if (!DEV_Digital_Read(keyB))
		{
			if (display_dim_idx >= display_dim_max)
			{
				display_dim_idx = 0;
				display_toggle = false;
			}
			else
			{
				++display_dim_idx;
				display_toggle = true;
			}
			set_brightness(display_dim[display_dim_idx]);
			sleep_ms(500);
		}

		new_time = get_absolute_time();

		if (is_connected && ((new_time - old_time) > REQUEST_DELAY))
		{
			old_time = new_time;

			uint16_t port = atoi(wifi_setting.server_port);

			err_t err = ERR_OK;

#ifdef USE_HTTP
			// // PATCHWORK!
			// if (continuous_unresolved_request_count >= UNRESOLVED_REQUEST_COUNT_THRESHOLD)
			// {
			// 	printf(PRINT_RED "\nReached unresolved request threshold [%d], resetting network connection...\n\n" PRINT_RESET, continuous_unresolved_request_count);
			// 	continuous_unresolved_request_count = 0;
			// 	++total_lwip_resets;

			// 	// strncpy(HEADING, STR_RESET_HEADING, strlen(STR_RESET_HEADING));
			// 	// HEADING[strlen(STR_RESET_HEADING)] = 0;

			// 	process_event(EVENT_WIFI_CONNECTED);

			// 	// process_event(EVENT_WIFI_DISCONNECTED); // not satisfactory

			// 	// watchdog_enable(0, 0); // NOPE!!!

			// 	// process_event(DEVICE_WIFI_LINK_DOWN); // useless!!! (It's obvious now)

			// 	// AIRCR_Register = 0x5FA0004; // doesn't work, most probably due to core affinity, resetting only its own core.

			// 	continue;
			// }

			// if ((err = httpc_get_file_dns(
			// 		 wifi_setting.server_ip_address,
			// 		 port,
			// 		 "/",
			// 		 &settings,
			// 		 recv_fn,
			// 		 NULL,
			// 		 &connection)) == ERR_OK)
			// {
			// 	++total_requests_sent;
			// 	++continuous_unresolved_request_count;
			// }
#else  // USE_HTTP
			if ((err = http_req_tcp_send(wifi_setting.server_ip_address, port)) == ERR_OK)
			{
				++total_requests_sent;
				++continuous_unresolved_request_count;
			}
#endif // USE_HTTP

			printf(PRINT_RESET "\nRequest [%d] sent to: http://%s:%d\nhttp_req_tcp_send() Err [%d]: %s\n" PRINT_RESET,
				   total_requests_sent,
				   wifi_setting.server_ip_address,
				   port,
				   err,
				   lwip_strerr(err));
		}
	}
}
