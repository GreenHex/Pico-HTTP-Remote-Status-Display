/**
 * json_proc.c
 * Copyright (c) 2025 MVK
 * 14-May-2025
 *
 */

#include "json_proc.h"
#include "../tiny-json/tiny-json.h"

#include <strings.h>
#include <string.h>
#include "math.h"

uint total_requests_sent = 0;
uint total_resolved_requests = 0;
uint continuous_unresolved_request_count = 0;
const uint UNRESOLVED_REQUEST_COUNT_THRESHOLD = 5;
uint total_lwip_resets = 0;

// LCD Display
#define LCD_LEFT_MARGIN 5
#define LCD_TOP_MARGIN 12
#define LCD_LINE_GAP 2

#define LINE_WIDTH_ADJUSTMENT 5
#define LINE_HEIGHT_ADJUSTMENT 5

extern UWORD *BlackImage;

#define MAX_STRING_LENGTH 32
#define MAX_LEN 1024

char lcd_buff[NUM_LCD_CHARS_PER_LINE];

char myBuff[BUFF_SIZE];

// tiny-json
enum
{
	MAX_FIELDS = 26
}; // MAX. JSON FIELDS

void display_LCD(char *json_str)
{
	static json_t pool[MAX_FIELDS];

	json_t const *parent = json_create(json_str, pool, MAX_FIELDS);
	if (parent)
	{ // No error checking, of-course!

		const char F_TIME[] = "TIME";
		const char F_IP_ADDRESS[] = "IP_ADDRESS";
		const char F_UPTIME[] = "UPTIME";
		const char F_LOAD[] = "LOAD";
		const char F_CPU_TEMP[] = "CPU_TEMP";
		const char F_CHARGE[] = "CHARGE";
		const char F_UPS_TIME[] = "UPS_TIME";
		//
		const char F_ON_BATTERY[] = "ON_BATTERY";								// bool
		const char F_BATTERY_PERCENT[] = "BATTERY_PERCENT";						// int
		const char F_NET_STATUS[] = "NET_STATUS";								// bool
		const char F_TIME_REMAINING_OR_TO_FULL[] = "TIME_REMAINING_OR_TO_FULL"; // int
		const char F_PROCESS_NAME[] = "PROCESS_NAME";							// string
		const char F_PROCESS_STATUS[] = "PROCESS_STATUS";
		//
		json_t const *p_time = json_getProperty(parent, F_TIME);
		json_t const *p_ip_address = json_getProperty(parent, F_IP_ADDRESS);
		json_t const *p_uptime = json_getProperty(parent, F_UPTIME);
		json_t const *p_load = json_getProperty(parent, F_LOAD);
		json_t const *p_cpu_temp = json_getProperty(parent, F_CPU_TEMP);
		json_t const *p_charge = json_getProperty(parent, F_CHARGE);
		json_t const *p_ups_time = json_getProperty(parent, F_UPS_TIME);
		json_t const *p_on_battery = json_getProperty(parent, F_ON_BATTERY);
		json_t const *p_battery_percent = json_getProperty(parent, F_BATTERY_PERCENT);
		json_t const *p_net_status = json_getProperty(parent, F_NET_STATUS);
		json_t const *p_time_remaining_or_to_full = json_getProperty(parent, F_TIME_REMAINING_OR_TO_FULL);
		json_t const *p_process_name = json_getProperty(parent, F_PROCESS_NAME);
		json_t const *p_process_status = json_getProperty(parent, F_PROCESS_STATUS);
		//
		char const *r_time = json_getValue(p_time);
		char const *r_ip_address = json_getValue(p_ip_address);
		char const *r_uptime = json_getValue(p_uptime);
		char const *r_load = json_getValue(p_load);
		char const *r_cpu_temp = json_getValue(p_cpu_temp);
		char const *r_charge = json_getValue(p_charge);
		char const *r_ups_time = json_getValue(p_ups_time);
		bool r_on_battery = json_getBoolean(p_on_battery);
		int r_battery_percent = json_getInteger(p_battery_percent);
		bool r_net_status = json_getBoolean(p_net_status);
		int r_time_remaining_or_to_full = json_getInteger(p_time_remaining_or_to_full);
		char const *r_process_name = json_getValue(p_process_name);
		bool r_process_status = json_getBoolean(p_process_status);
		//
		char cur_time[MAX_STRING_LENGTH] = "\0";
		char ip_address[MAX_STRING_LENGTH] = "\0";
		char uptime[MAX_STRING_LENGTH] = "\0";
		char load[MAX_STRING_LENGTH] = "\0";
		char cpu_temp[MAX_STRING_LENGTH] = "\0";
		char charge[MAX_STRING_LENGTH] = "\0";
		char ups_time[MAX_STRING_LENGTH] = "\0";
		//
		bool on_battery = false;
		int battery_percent = 0;
		bool net_status = false;
		int time_remaining_or_to_full = 0;
		char process_name[MAX_STRING_LENGTH] = "\0";
		bool process_status = false;

		strncpy(cur_time, r_time, MAX_STRING_LENGTH - 1);
		strncpy(ip_address, r_ip_address, MAX_STRING_LENGTH - 1);
		strncpy(uptime, r_uptime, MAX_STRING_LENGTH - 1);
		strncpy(load, r_load, MAX_STRING_LENGTH - 1);
		strncpy(cpu_temp, r_cpu_temp, MAX_STRING_LENGTH - 1);
		strncpy(charge, r_charge, MAX_STRING_LENGTH - 1);
		strncpy(ups_time, r_ups_time, MAX_STRING_LENGTH - 1);
		on_battery = r_on_battery;
		battery_percent = r_battery_percent;
		net_status = r_net_status;
		time_remaining_or_to_full = r_time_remaining_or_to_full;
		strncpy(process_name, r_process_name, MAX_STRING_LENGTH - 1);
		process_status = r_process_status;

		// dump(parent);

		UWORD batt_colour = BLACK;

		if (battery_percent > 50)
			batt_colour = GREEN;
		else if (battery_percent > 20)
			batt_colour = YELLOW;
		else
			batt_colour = RED;

		Paint_Clear(BLACK);
		Paint_DrawRectangle(2, 2, LCD_1IN3_WIDTH - 2, 28, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		Paint_DrawString_EN((LCD_1IN3_WIDTH - strlen(cur_time) * Font20.Width) / 2, 7, cur_time, &Font20, BLACK, WHITE);
		//
		Paint_DrawString_EN(5, 37, "IP Address", &Font12, MAGENTA, BLACK);
		Paint_DrawString_EN(LCD_1IN3_WIDTH - strlen(ip_address) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 57, ip_address, &Font20, WHITE, BLACK);
		Paint_DrawLine(5, 57 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH - 5, 57 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		//
		Paint_DrawString_EN(5, 87, "Uptime", &Font12, MAGENTA, BLACK);
		Paint_DrawString_EN(LCD_1IN3_WIDTH - strlen(uptime) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 107, uptime, &Font20, WHITE, BLACK);
		Paint_DrawLine(5, 107 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH - 5, 107 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		//
		Paint_DrawString_EN(5, 137, "CPU Load", &Font12, MAGENTA, BLACK);
		Paint_DrawString_EN(LCD_1IN3_WIDTH / 2 - strlen(load) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 157, load, &Font20, WHITE, BLACK);
		//
		Paint_DrawString_EN(5 + LCD_1IN3_WIDTH / 2, 137, on_battery ? "On Battery" : "Battery", &Font12, MAGENTA, BLACK);
		Paint_DrawString_EN(LCD_1IN3_WIDTH - strlen(charge) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 157, charge, &Font20, batt_colour, BLACK);
		Paint_DrawLine(5, 157 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH / 2, 157 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(LCD_1IN3_WIDTH / 2, 157 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH - 5, 157 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		//
		Paint_DrawString_EN(5, 187, "CPU Temperature", &Font12, MAGENTA, BLACK);
		Paint_DrawString_EN(LCD_1IN3_WIDTH / 2 - strlen(cpu_temp) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 207, cpu_temp, &Font20, WHITE, BLACK);
		//
		Paint_DrawString_EN(5 + LCD_1IN3_WIDTH / 2, 187, on_battery ? "Time To Empty" : "Time To Full", &Font12, MAGENTA, BLACK);
		Paint_DrawString_EN(LCD_1IN3_WIDTH - strlen(ups_time) * Font20.Width - LINE_WIDTH_ADJUSTMENT, 207, ups_time, &Font20, on_battery & (time_remaining_or_to_full < 20) ? RED : WHITE, BLACK);
		Paint_DrawLine(5, 207 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, LCD_1IN3_WIDTH - 5, 207 + Font20.Height + LINE_HEIGHT_ADJUSTMENT, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		//
		Paint_DrawLine(LCD_1IN3_WIDTH / 2, 132, LCD_1IN3_WIDTH / 2, 240 - 8, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		//
		if (!net_status)
		{
			Paint_DrawChar(10, 57, '~', &Font20, BLACK, GREEN);
		}
		else
		{
			Paint_DrawChar(10, 57, 'X', &Font20, BLACK, RED);
		}
		LCD_1IN3_Display(BlackImage);
	}
	return;
}

int LCD_print(int line_number, char *str, UWORD colour)
{
	if (line_number < 1)
		line_number = 1;

	if (!colour)
		colour = BLACK;

	int number_of_lines_printed = 1;
	int str_print_size = strlen(str) * Font20.Width;

	number_of_lines_printed += ceil(str_print_size / LCD_1IN3_WIDTH);

	Paint_DrawString_EN(LCD_LEFT_MARGIN, (Font20.Height + LCD_LINE_GAP) * line_number + LCD_TOP_MARGIN, str, &Font20, colour, WHITE);

	LCD_1IN3_Display(BlackImage);

	return number_of_lines_printed;
}

char HEADING[16] = "DEVICE CONFIG";

void LCD_print_next(char *str, UWORD colour)
{
	static int line_number = 1;

	if (line_number > 9)
	{
		line_number = 1;
		Paint_Clear(WHITE);
		Paint_DrawRectangle(2, 2, LCD_1IN3_WIDTH - 2, 28, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		Paint_DrawString_EN((LCD_1IN3_WIDTH - strlen(HEADING) * Font20.Width) / 2, 7, HEADING, &Font20, WHITE, BLACK);
		LCD_1IN3_Display(BlackImage);
	}

	if (!colour)
		colour = BLACK;

	line_number += LCD_print(line_number, str, colour);

	return;
}
