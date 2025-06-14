/**
 * ble.h
 * Copyright (c) 2025 MVK
 * 09-May-2025
 *
 */

#ifndef _BLE_H_
#define _BLE_H_

#include "../lib/LCD/LCD_1in3.h"
#include "../src/LCD_Test.h"
#include <stdio.h>
#include "math.h"

#define PRINT_RESET "\033[0m"
#define PRINT_RED "\033[31m"	/* Red */
#define PRINT_GREEN "\033[32m"	/* Green */
#define PRINT_YELLOW "\033[33m" /* Yellow */
#define PRINT_CYAN "\033[36m"	/* Cyan */

typedef enum
{
	DEVICE_START_UP = 0,
	DEVICE_WIFI_LINK_DOWN,
	DEVICE_WIFI_LINK_TO_UP,
	DEVICE_WIFI_LINK_UP,
	DEVICE_WIFI_LINK_CONNECTED,
	DEVICE_RUNNING,
	DEVICE_ERROR
} device_state_t;

typedef enum
{
	EVENT_NONE = 0,
	EVENT_WIFI_CONFIGURED,
	EVENT_WIFI_CONNECT,
	EVENT_WIFI_CONNECTED,
	EVENT_WIFI_ERROR,
	EVENT_IP_ACQUIRED,
	EVENT_WIFI_DISCONNECTED,
	EVENT_ERROR_OCCURED
} device_event_t;

typedef struct
{
	char ssid[36];
	char password[64];
	char device_ip_address[16];
	char server_ip_address[16];
	char server_port[16];
	int rand_check_num; // 4 Bytes
	char padding[256 - (36 + 64 + 16 + 16 + 16 + 4)];
} wifi_setting_t;

void process_event(device_event_t event);
int start_ble(void);

#endif //  _BLE_H_
