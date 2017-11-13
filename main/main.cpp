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


#define SKELETOR_DEBUG
#define SKELETOR_FEATURE_SHELLARDUINO
#define SKELETOR_FEATURE_SSD1306ARDUINO
#define SKELETOR_FEATURE_WIFIARDUINO
#define SKELETOR_FEATURE_MQTTARDUINO


#define APP_TASKS_RUNNING_CORE 		1


//////////////////////////////		PROJECT DEBUG		//////////////////////////////
void debugPrint(String message)
{
	//	if(!debug_enabled) return; // debug is disabled.

	// if UDP is connected log over UDP.
	//sendToUDP(message);

	// log over serial anyway
	Serial.print(message);
}


#ifdef SKELETOR_DEBUG
#define SKELETOR_DEBUG_PRINT(...) { debugPrint(__VA_ARGS__); }
#else
#define SKELETOR_DEBUG_PRINT(...) {}
#endif
//////////////////////////////		PROJECT DEBUG		//////////////////////////////



#ifdef SKELETOR_FEATURE_SHELLARDUINO
//////////////////////////////		NTSHELL ARDUINO		//////////////////////////////
#include "ntshell_arduino.h"
//////////////////////////////		NTSHELL ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SHELLARDUINO



#ifdef SKELETOR_FEATURE_WIFIARDUINO
//////////////////////////////		WIFI ARDUINO		//////////////////////////////
#include "wifi_management_arduino.h"
//////////////////////////////		WIFI ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_WIFIARDUINO



#ifdef SKELETOR_FEATURE_MQTTARDUINO
//////////////////////////////		MQTT ARDUINO		//////////////////////////////
#include "mqtt_arduino.h"
//////////////////////////////		MQTT ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_MQTTARDUINO



#ifdef SKELETOR_FEATURE_SSD1306ARDUINO
//////////////////////////////		SSD1306 ARDUINO		//////////////////////////////
#include "ssd1306_arduino.h"
//////////////////////////////		SSD1306 ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SSD1306ARDUINO






TaskHandle_t xTaskHandle_worker_b = NULL;

#include "external_ip_detect_arduino.h"
#include "ssd1306_arduino.h"
// worker task b -> gets the current external ip address and displays it on the ssd1306 display.
void worker_task_b(void *pvParameter)
{
	uint32_t ulNotifiedValue;
	String externalIPAddress;

	xTaskHandle_worker_b = xTaskGetCurrentTaskHandle();	/* set our handle here */

	while(1)
	{
		//printf("worker_task_b: %d\n", i);

		/* wait to be notified when to proceed further */
		xTaskNotifyWait(
				0x00,      			/* Don't clear any notification bits on entry. */
				ULONG_MAX, 			/* Reset the notification value to 0 on exit. */
				&ulNotifiedValue, 	/* Notified value pass out in ulNotifiedValue. */
				portMAX_DELAY		/* Block indefinitely. */
				);

		if( ( ulNotifiedValue & 0x01 ) != 0 )
		{
			// invoked the external ip address detect
			externalIPAddress = detectExternalIP();

			// display the external ip address on the oled
			displayExternalIPAddress(externalIPAddress);
		}
		else if( ( ulNotifiedValue & 0x02 ) != 0 )
		{
			//displayCount();
		}
		else
		{
			// continue forward anyway. can be handled differently. will expand later.
		}
	}
}


TaskHandle_t xTaskHandle_worker_a = NULL;

// worker task a -> waits until it is notified that system is ready.
// this then runs a loop with sleep time delay (30 sec) and wakes up another task
// worker task b to do its job in every loop
void worker_task_a(void *pvParameter)
{
	uint32_t ulNotifiedValue;
	uint32_t ulDelayInSeconds = 30;

	uint32_t i = 0;

	xTaskHandle_worker_a = xTaskGetCurrentTaskHandle();	/* set our handle here */

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
		//printf("worker_task_a: %d\n", i);

		// send task notification to worker tasks, setup is done.
		/* Set bit 1 in the notification value of the task referenced by xTask1Handle. */
		if(xTaskNotify( xTaskHandle_worker_b, ( 1UL ), eSetBits ) == pdPASS)
		{
			/* The task's notification value was updated. */
			//printf("\n\nworker task a notified ok.\n");
			//displayCount(i);
		}
		else
		{
			/* The task's notification value was not updated. */
			printf("\n\nworker task b notification failed.\n");
		}


		vTaskDelay((ulDelayInSeconds * 1000) / portTICK_RATE_MS); // 30 seconds
		i+=ulDelayInSeconds;
	}
}



//////////////////////////////		ARDUINO TASK		//////////////////////////////
void setup()
{
	Serial.begin(115200);


#ifdef SKELETOR_DEBUG
	//////////////////////////////		DEBUG		//////////////////////////////
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
	//////////////////////////////		DEBUG		//////////////////////////////
#endif // #ifdef SKELETOR_DEBUG


#ifdef SKELETOR_FEATURE_SHELLARDUINO
	//////////////////////////////		NTSHELL ARDUINO		//////////////////////////////
	initialize_shell();
	//////////////////////////////		NTSHELL ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SHELLARDUINO


#ifdef SKELETOR_FEATURE_SSD1306ARDUINO
	//////////////////////////////		SSD1306 ARDUINO		//////////////////////////////
	configureSSD1306Display();
	//////////////////////////////		SSD1306 ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SSD1306ARDUINO


#ifdef SKELETOR_FEATURE_WIFIARDUINO
	//////////////////////////////		WIFI ARDUINO		//////////////////////////////
	configureWiFiAPMode();
	delay(1000); // without this the AP START event does not happen ? KK TODO check this, possible bug.
	configureWiFiSTAMode();
	//////////////////////////////		WIFI ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_WIFIARDUINO


#ifdef SKELETOR_FEATURE_MQTTARDUINO
	//////////////////////////////		MQTT ARDUINO		//////////////////////////////
	configureMqtt();
	//////////////////////////////		MQTT ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_MQTTARDUINO


	// send task notification to worker task a, initial setup is done.
	/* Set bit 1 in the notification value of the task referenced by xTaskHandle_worker_a. */
	if(xTaskNotify( xTaskHandle_worker_a, ( 1UL ), eSetBits ) == pdPASS)
	{
		/* The task's notification value was updated. */
		//printf("\n\nworker task b notified ok.\n");
	}
	else
	{
		/* The task's notification value was not updated. */
		printf("\n\nworker task b notification failed.\n");
	}
}


void loop()
{

#ifdef SKELETOR_FEATURE_SHELLARDUINO
	//////////////////////////////		NTSHELL ARDUINO		//////////////////////////////
	poll_shell();
	//////////////////////////////		NTSHELL ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SHELLARDUINO

}


// this is the standard arduino freertos task template, no need to change.
void arduinoTask(void *pvParameters)
{
	setup();

	for(;;)
	{
		micros(); // update overflow
		loop();
	}
}
//////////////////////////////		ARDUINO TASK		//////////////////////////////


//////////////////////////////		ENTRY POINT		//////////////////////////////
extern "C" void app_main()
{
	esp_err_t ret;

	ret = nvs_flash_init();
	if(ret != ESP_OK)
	{
		printf("nvs_flash_init FAILED! error code = [0x%X]\n",ret);
	}
	else
	{
		printf("nvs_flash_init ok.\n");
	}


	xTaskCreatePinnedToCore(worker_task_a, "worker_task_a", 2048, NULL, 1, NULL, APP_TASKS_RUNNING_CORE);
	xTaskCreatePinnedToCore(worker_task_b, "worker_task_b", 2048, NULL, 1, NULL, APP_TASKS_RUNNING_CORE);

	xTaskCreatePinnedToCore(arduinoTask, "arduinoTask", 8192, NULL, 1, NULL, APP_TASKS_RUNNING_CORE);
}
//////////////////////////////		ENTRY POINT		//////////////////////////////
