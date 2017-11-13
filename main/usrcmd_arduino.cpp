
/**
   @file usrcmd.c
   @author CuBeatSystems
   @author Shinichiro Nakamura
   @copyright
   ===============================================================
   Natural Tiny Shell (NT-Shell) Version 0.3.1
   ===============================================================
   Copyright (c) 2010-2016 Shinichiro Nakamura

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following
   conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ntopt.h"
#include "ntlibc.h"
#include <Arduino.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#endif // #ifdef __AVR__

#include "usrcmd_arduino.h"

#define SYSINFOSTR "ESP32 VT100\r\n"
#define VERSIONSTR "1.0 October 2, 2017\r\n"




#define uart_puts Serial.print

void pgm_print(const char *p)
{
#ifdef __AVR__
	char c;
	while ((c = pgm_read_byte(p++)) != 0)
		Serial.write(c);
#else // #ifdef __AVR__
	Serial.write(p);
#endif // #ifdef __AVR__
}

typedef int (*USRCMDFUNC)(int argc, char **argv);

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);
static int usrcmd_help(int argc, char **argv);
static int usrcmd_info(int argc, char **argv);
static int usrcmd_oled(int argc, char **argv);
static int usrcmd_extip(int argc, char **argv);

#ifdef __AVR__
const char cmd_0[] PROGMEM = 	"help";
const char desc_0[] PROGMEM = 	"show help";
const char cmd_1[] PROGMEM = 	"info";
const char desc_1[] PROGMEM = 	"show info";

typedef struct {
	PROGMEM const char *cmd;
	PROGMEM const char *desc;
	USRCMDFUNC func;
} cmd_table_t;


static const cmd_table_t cmdlist[] PROGMEM = {
		{ cmd_0, desc_0, usrcmd_help },
		{ cmd_1, desc_1, usrcmd_info },
};
#else // #ifdef __AVR__
const char cmd_0[] = 	"help";
const char desc_0[] = 	"show help";
const char cmd_1[] = 	"info";
const char desc_1[] = 	"show info";
const char cmd_2[] = 	"oled";
const char desc_2[] = 	"display on oled";
const char cmd_3[] = 	"externalip";
const char desc_3[] = 	"detect wan side ip address";

typedef struct {
	const char *cmd;
	const char *desc;
	USRCMDFUNC func;
} cmd_table_t;


static const cmd_table_t cmdlist[] = {
		{ cmd_0, desc_0, usrcmd_help },
		{ cmd_1, desc_1, usrcmd_info },
		{ cmd_2, desc_2, usrcmd_oled },
		{ cmd_3, desc_3, usrcmd_extip },
};
#endif // #ifdef __AVR__



int usrcmd_execute(const char *text)
{
	return ntopt_parse(text, usrcmd_ntopt_callback, 0);
}

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
	if (argc == 0)
	{
		return 0;
	}
	cmd_table_t *p;
	char cmd[MAX_CMD_LEN];
	p = (cmd_table_t *) &cmdlist[0];
	for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++)
	{
#ifdef __AVR__
		char *pcmd = (char *)pgm_read_word(&((*p).cmd));
		USRCMDFUNC pfunc = (USRCMDFUNC) pgm_read_word(&((*p).func));
		strcpy_P(cmd, pcmd);
#else // #ifdef __AVR__
		const char *pcmd = ((*p).cmd);
		USRCMDFUNC pfunc = (USRCMDFUNC) ((*p).func);
		strcpy(cmd, pcmd);
#endif // #ifdef __AVR__

		if (ntlibc_strcmp((const char *)argv[0], cmd) == 0)
		{
			return (*pfunc)(argc, argv);
		}
		p++;
	}
	uart_puts(F("Unknown command\r\n"));
	Serial.flush();
	return 0;
}

static int usrcmd_help(int argc, char **argv)
{
	const cmd_table_t *p = &cmdlist[0];
	for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++)
	{
#ifdef __AVR__
		char *pcmd = (char *)pgm_read_word(&((*p).cmd));
		char *pdesc = (char *)pgm_read_word(&((*p).desc));
#else // #ifdef __AVR__
		const char *pcmd = ((*p).cmd);
		const char *pdesc = ((*p).desc);
#endif // #ifdef __AVR__
		pgm_print(pcmd);
		uart_puts("\t:");
		pgm_print(pdesc);
		uart_puts("\r\n");
		p++;
	}
	Serial.flush();
	return 0;
}

static int usrcmd_info(int argc, char **argv)
{
	if (argc != 2)
	{
		uart_puts(F("info sys\r\n"));
		uart_puts(F("info ver\r\n"));
		return 0;
	}
	if (ntlibc_strcmp(argv[1], "sys") == 0)
	{
		uart_puts(SYSINFOSTR);
		return 0;
	}
	if (ntlibc_strcmp(argv[1], "ver") == 0)
	{
		uart_puts(VERSIONSTR);
		return 0;
	}
	uart_puts(F("Unknown sub command found\r\n"));
	return -1;
}

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
extern SSD1306 display;
int usrcmd_oled(int argc, char **argv)
{
	if (argc != 2)
	{
		uart_puts(F("oled <string>\r\n"));
		return 0;
	}



	display.setColor(BLACK);
	display.fillRect(0, 0, 128, 20); // clear the area.

	display.setColor(WHITE);
	display.drawString(5,5, String(argv[1]));
	display.display();

	return 0;
}


#include "external_ip_detect_arduino.h"
int usrcmd_extip(int argc, char **argv)
{
	String externalIPAddress;

	if (argc != 1)
	{
		uart_puts(F("externalip <no-arguments>\r\n"));
		return 0;
	}

	externalIPAddress = detectExternalIP();

	//uart_puts("current ip address: ");
	//uart_puts(externalIPAddress);
	//uart_puts("\r\n");

	return 0;
}

