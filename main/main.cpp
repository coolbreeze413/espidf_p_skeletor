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


