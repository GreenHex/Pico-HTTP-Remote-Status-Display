/**
 * json_proc.h
 * Copyright (c) 2025 MVK
 * 14-May-2025
 *
 */

#ifndef _JSON_PROC_H_
#define _JSON_PROC_H_

#include "../src/LCD_Test.h"
#include "../lib/LCD/LCD_1in3.h"

#define BUFF_SIZE 2048
extern char myBuff[];

#define NUM_LCD_CHARS_PER_LINE 16
extern char lcd_buff[NUM_LCD_CHARS_PER_LINE];

void display_LCD(char *json_str);
int LCD_print(int line_number, char *str, UWORD colour);
void LCD_print_next(char *str, UWORD colour);

#endif // _JSON_PROC_H_
