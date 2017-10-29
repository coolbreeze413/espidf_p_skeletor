#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "Arduino.h"

////////////////////////////// NTSHELL
#include "ntshell.h"
#include "ntlibc.h"

#include <usrcmd_arduino.h>
extern "C" {
#include <ntshell_arduino.h>

}



#define PROMPTSTR "coolbreeze>"

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
#if 0
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



////////////////////////////// NTSHELL


///////////////////////////// DISPLAY
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
#include "images.h"

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

///////////////////////////// DISPLAY






////////////// CONFIGS

void debugPrint(String message) {

	//	if(!debug_enabled) return; // debug is disabled.

	// if UDP is connected log over UDP.
	//sendToUDP(message);

	// log over serial anyway
	Serial.print(message);

}


#define X_PROJECT_DEBUG
#ifdef X_PROJECT_DEBUG
	#define X_PROJECT_DEBUG_PRINT(...) { debugPrint(__VA_ARGS__); }
#else
	#define X_PROJECT_DEBUG_PRINT(...) {}
#endif
/////////////





void hello_task(void *pvParameter)
{
	// dummy task for load.
	uint8_t i = 0;
    printf("hello_task started!\n");

    while(1)
    {
        printf("hello_task: %d\n", i);

        display.clear();
        display.drawString(64,22, String("loop: ") + String(i));
        display.display();

        vTaskDelay(1000 / portTICK_RATE_MS);
        i++;
    }
}








void setup(){
  Serial.begin(115200);


	/////////////////////////////////////////////////////
#ifdef X_PROJECT_DEBUG
	X_PROJECT_DEBUG_PRINT("\n\n");
	delay(500);
	X_PROJECT_DEBUG_PRINT("███████╗███████╗██████╗ ██████╗ ██████╗     ██╗   ██╗████████╗ ██╗ ██████╗  ██████╗ \n");
	delay(500);
	X_PROJECT_DEBUG_PRINT("██╔════╝██╔════╝██╔══██╗╚════██╗╚════██╗    ██║   ██║╚══██╔══╝███║██╔═████╗██╔═████╗\n");
	delay(500);
	X_PROJECT_DEBUG_PRINT("█████╗  ███████╗██████╔╝ █████╔╝ █████╔╝    ██║   ██║   ██║   ╚██║██║██╔██║██║██╔██║\n");
	delay(500);
	X_PROJECT_DEBUG_PRINT("██╔══╝  ╚════██║██╔═══╝  ╚═══██╗██╔═══╝     ╚██╗ ██╔╝   ██║    ██║████╔╝██║████╔╝██║\n");
	delay(500);
	X_PROJECT_DEBUG_PRINT("███████╗███████║██║     ██████╔╝███████╗     ╚████╔╝    ██║    ██║╚██████╔╝╚██████╔╝\n");
	delay(500);
	X_PROJECT_DEBUG_PRINT("╚══════╝╚══════╝╚═╝     ╚═════╝ ╚══════╝      ╚═══╝     ╚═╝    ╚═╝ ╚═════╝  ╚═════╝ \n");
	delay(500);
	X_PROJECT_DEBUG_PRINT("\n\n");
#endif // #ifdef X_PROJECT_DEBUG
	/////////////////////////////////////////////////////


	//////////////////////////////////////////// NTSHELL
	ntshell_init(
			&ntshell,
			func_read,
			func_write,
			func_callback,
			(void *)(&ntshell));

	ntshell_set_prompt(&ntshell, PROMPTSTR);
	Serial.println(F("Welcome to ESP32 VT100.\r\n type 'help' for help."));
	Serial.print(PROMPTSTR);
	Serial.flush();
	//////////////////////////////////////////// NTSHELL


	//////////////////////////////////////////// DISPLAY
	// Initialising the UI will init the display too.
	display.init();

	//display.flipScreenVertically();
	display.setFont(ArialMT_Plain_10);

	// clear the display
	display.clear();


	//display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);

	display.flipScreenVertically();
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.drawString(64,22, "KaNTHRi PoDaPaTi");

	// write the buffer to the display
	display.display();

	vTaskDelay(3000 / portTICK_RATE_MS);
	//////////////////////////////////////////// DISPLAY

	xTaskCreatePinnedToCore(hello_task, "hello_task", 2048, NULL, 1, NULL, 0); // dummy task on Core 0.

}

void loop(){

	//////////////////////////////////////////// NTSHELL
	while(Serial.available())
	{
		ntshell_execute_arduino(&ntshell);
	}
	//////////////////////////////////////////// NTSHELL



}


#if 0//CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else // always run arduino stuff on core 1
#define ARDUINO_RUNNING_CORE 1 
#endif







#if 0//CONFIG_AUTOSTART_ARDUINO


#else // always no autostart arduino - manual contol

void loopTask(void *pvParameters)
{
    setup();
    for(;;)
    {
        micros(); //update overflow
        loop();
    }
}


extern "C" void app_main()
{
	nvs_flash_init();
	xTaskCreatePinnedToCore(loopTask, "loopTask", 8192, NULL, 1, NULL, ARDUINO_RUNNING_CORE);

}


#endif


