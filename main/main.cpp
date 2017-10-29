//
//  ███████╗██╗  ██╗███████╗██╗     ███████╗████████╗ ██████╗ ██████╗
//  ██╔════╝██║ ██╔╝██╔════╝██║     ██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗
//  ███████╗█████╔╝ █████╗  ██║     █████╗     ██║   ██║   ██║██████╔╝
//  ╚════██║██╔═██╗ ██╔══╝  ██║     ██╔══╝     ██║   ██║   ██║██╔══██╗
//  ███████║██║  ██╗███████╗███████╗███████╗   ██║   ╚██████╔╝██║  ██║
//  ╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝
//


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "Arduino.h"

#define SKELETOR_FEATURE_SHELL
#define SKELETOR_FEATURE_SSD1306


#define APP_TASKS_RUNNING_CORE 		1


// synchronization via task notification
TaskHandle_t xTaskToNotify = NULL;



#ifdef SKELETOR_FEATURE_SHELL
//////////////////////////////		NTSHELL		//////////////////////////////
#include "ntshell.h"
#include "ntlibc.h"

#include <usrcmd_arduino.h>
extern "C" {
#include <ntshell_arduino.h>

}

#define PROMPTSTR "skeletor>"

static ntshell_t ntshell;


static int func_read(char *buf, int cnt, void *extobj)
{
	if (Serial.available())
	{
		return Serial.readBytes(buf, cnt);
	}
	else
	{
		return 0;
	}
}

static int func_write(const char *buf, int cnt, void *extobj)
{
	return Serial.write((const uint8_t*)buf, cnt);
}

static int func_callback(const char *text, void *extobj)
{
#if 0 // local testing.
	ntshell_t *ntshell = (ntshell_t *)extobj;
	//UNUSED_VARIABLE(ntshell);
	//UNUSED_VARIABLE(extobj);
	if (ntlibc_strlen(text) > 0) {
		Serial.print("User input text:");
		Serial.print(text);
		Serial.print("\r\n");
	}
	return 0;
#else
	return usrcmd_execute(text);
#endif
}
//////////////////////////////		NTSHELL		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SHELL



#ifdef SKELETOR_FEATURE_SSD1306
//////////////////////////////		SSD1306 DISPLAY		//////////////////////////////
// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
// or #include "SH1106.h" alis for `#include "SH1106Wire.h"`
// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"

// Include custom images
//#include "images.h"

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5, 4);
// SH1106 display(0x3c, D3, D5);
//////////////////////////////		SSD1306 DISPLAY		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SSD1306


////////////// CONFIGS

void debugPrint(String message)
{
	//	if(!debug_enabled) return; // debug is disabled.

	// if UDP is connected log over UDP.
	//sendToUDP(message);

	// log over serial anyway
	Serial.print(message);

}


#define SKELETOR_DEBUG
#ifdef SKELETOR_DEBUG
#define SKELETOR_DEBUG_PRINT(...) { debugPrint(__VA_ARGS__); }
#else
#define SKELETOR_DEBUG_PRINT(...) {}
#endif
/////////////





void hello_task(void *pvParameter)
{
	// dummy task for load.
	uint32_t ulNotifiedValue;

	uint8_t i = 0;
	//printf("hello_task started!\n\n");

	xTaskToNotify = xTaskGetCurrentTaskHandle();	/* set our handle here */

	/* wait to be notified when to proceed further */
	xTaskNotifyWait(
			0x00,      			/* Don't clear any notification bits on entry. */
			ULONG_MAX, 			/* Reset the notification value to 0 on exit. */
			&ulNotifiedValue, 	/* Notified value pass out in ulNotifiedValue. */
			portMAX_DELAY		/* Block indefinitely. */
			);

	if( ( ulNotifiedValue & 0x01 ) != 0 )
	{
		 /* Bit 0 was set */
		 // continue forward
	}
	else
	{
		// continue forward anyway. can be handled differently. will expand later.
	}

	while(1)
	{
		//printf("hello_task: %d\n", i);

		// TODO remove hardcoded coords and make a generic UI.
		display.setColor(BLACK);
		display.fillRect(60, 42, 128, 20); // clear the area.
		display.setColor(WHITE);
		display.drawString(80,42, String(i));
		display.display();

		vTaskDelay(1000 / portTICK_RATE_MS);
		i++;
	}
}







//////////////////////////////		ARDUINO TASK		//////////////////////////////
void setup()
{
	Serial.begin(115200);


	/////////////////////////////////////////////////////
#ifdef SKELETOR_DEBUG
	SKELETOR_DEBUG_PRINT("\n\n");
	delay(500);
	SKELETOR_DEBUG_PRINT("███████╗██╗  ██╗███████╗██╗     ███████╗████████╗ ██████╗ ██████╗  \n");
	delay(500);
	SKELETOR_DEBUG_PRINT("██╔════╝██║ ██╔╝██╔════╝██║     ██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗\n");
	delay(500);
	SKELETOR_DEBUG_PRINT("███████╗█████╔╝ █████╗  ██║     █████╗     ██║   ██║   ██║██████╔╝\n");
	delay(500);
	SKELETOR_DEBUG_PRINT("╚════██║██╔═██╗ ██╔══╝  ██║     ██╔══╝     ██║   ██║   ██║██╔══██╗\n");
	delay(500);
	SKELETOR_DEBUG_PRINT("███████║██║  ██╗███████╗███████╗███████╗   ██║   ╚██████╔╝██║  ██║\n");
	delay(500);
	SKELETOR_DEBUG_PRINT("╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝ \n");
	delay(500);
	SKELETOR_DEBUG_PRINT("\n\n");
#endif // #ifdef SKELETOR_DEBUG
	/////////////////////////////////////////////////////


#ifdef SKELETOR_FEATURE_SHELL
	//////////////////////////////		NTSHELL		//////////////////////////////
	ntshell_init(
			&ntshell,
			func_read,
			func_write,
			func_callback,
			(void *)(&ntshell));

	ntshell_set_prompt(&ntshell, PROMPTSTR);
	Serial.println(F("welcome to s.k.e.l.e.t.o.r \r\n type 'help' for help."));
	Serial.print(PROMPTSTR);
	Serial.flush();
	//////////////////////////////		NTSHELL		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SHELL


#ifdef SKELETOR_FEATURE_SSD1306
	//////////////////////////////		SSD1306 DISPLAY		//////////////////////////////
	// Initialising the UI will init the display too.
	display.init();

	//display.flipScreenVertically();
	display.setFont(ArialMT_Plain_16);

	// clear the display
	display.flipScreenVertically();
	display.setContrast(255);
	display.clear();

	//display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);

	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.drawString(64, 22, "s.k.E.l.E.t.O.r");

	// write the buffer to the display
	display.display();
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	//////////////////////////////		SSD1306 DISPLAY		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SSD1306

	// send task notification to hello task, setup is done.
	/* Set bit 1 in the notification value of the task referenced by xTask1Handle. */
	if(xTaskNotify( xTaskToNotify, ( 1UL ), eSetBits ) == pdPASS)
	{
		/* The task's notification value was updated. */
		printf("\n\nhello_task notified ok.\n");
	}
	else
	{
		/* The task's notification value was not updated. */
		printf("\n\nhello_task notification failed.\n");
	}
}

void loop()
{

#ifdef SKELETOR_FEATURE_SHELL
	//////////////////////////////		NTSHELL		//////////////////////////////
	while(Serial.available())
	{
		ntshell_execute_arduino(&ntshell);
	}
	//////////////////////////////		NTSHELL		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SHELL

}


void arduinoTask(void *pvParameters)
{
	setup();
	for(;;)
	{
		micros(); //update overflow
		loop();
	}
}
//////////////////////////////		ARDUINO TASK		//////////////////////////////



extern "C" void app_main()
{
	nvs_flash_init();

	xTaskCreatePinnedToCore(hello_task, "hello_task", 2048, NULL, 1, NULL, APP_TASKS_RUNNING_CORE);

	xTaskCreatePinnedToCore(arduinoTask, "arduinoTask", 8192, NULL, 1, NULL, APP_TASKS_RUNNING_CORE);
}

