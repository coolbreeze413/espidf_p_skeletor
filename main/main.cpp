

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
#include "esp_task_wdt.h"
#include "Arduino.h"

#include <skeletor_config.h>

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
#define SKELETOR_DEBUG_PRINT(...) \
	{                             \
		debugPrint(__VA_ARGS__);  \
	}
#else
#define SKELETOR_DEBUG_PRINT(...) \
	{                             \
	}
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

#ifdef SKELETOR_FEATURE_LORA
//////////////////////////////		LORA ARDUINO		//////////////////////////////
#include "lora_arduino.h"
//////////////////////////////		LORA ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_LORA

#ifdef SKELETOR_FEATURE_SPIFFSARDUINO
//////////////////////////////		SPIFFS ARDUINO		//////////////////////////////
#include "spiffs_arduino.h"
//////////////////////////////		SPIFFS ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_SPIFFSARDUINO


#include "external_ip_detect_arduino.h"


TaskHandle_t xTaskHandle_worker_b = NULL;

// worker task b -> gets the current external ip address and displays it on the ssd1306 display.
void worker_task_b(void *pvParameter)
{
	uint32_t ulNotifiedValue;
	String externalIPAddress;

	xTaskHandle_worker_b = xTaskGetCurrentTaskHandle(); /* set our handle here */

	while (1)
	{
		//printf("worker_task_b: %d\n", i);

		/* wait to be notified when to proceed further */
		xTaskNotifyWait(
			0x00,			  /* Don't clear any notification bits on entry. */
			ULONG_MAX,		  /* Reset the notification value to 0 on exit. */
			&ulNotifiedValue, /* Notified value pass out in ulNotifiedValue. */
			portMAX_DELAY	 /* Block indefinitely. */
		);

		if ((ulNotifiedValue & 0x01) != 0)
		{
#ifdef SKELETOR_FEATURE_WIFIARDUINO
			// invoked the external ip address detect
			externalIPAddress = detectExternalIP();

#ifdef SKELETOR_FEATURE_SSD1306ARDUINO
			// display the external ip address on the oled
			displayExternalIPAddress(externalIPAddress);
#endif // #ifdef SKELETOR_FEATURE_SSD1306ARDUINO

#endif // #ifdef SKELETOR_FEATURE_WIFIARDUINO

#ifdef SKELETOR_FEATURE_LORA_SENDER
			sendLoRaPacket();
#endif // #ifdef SKELETOR_FEATURE_LORA_SENDER
		}
		else if ((ulNotifiedValue & 0x02) != 0)
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
#ifdef SKELETOR_FEATURE_LORA_SENDER
	uint32_t ulDelayInSeconds = 5;
#else  // #ifdef SKELETOR_FEATURE_LORA_SENDER
	uint32_t ulDelayInSeconds = 300;
#endif // #ifdef SKELETOR_FEATURE_LORA_SENDER
	uint32_t i = 0;

	xTaskHandle_worker_a = xTaskGetCurrentTaskHandle(); /* set our handle here */

	/* wait to be notified when to proceed further */
	xTaskNotifyWait(
		0x00,			  /* Don't clear any notification bits on entry. */
		ULONG_MAX,		  /* Reset the notification value to 0 on exit. */
		&ulNotifiedValue, /* Notified value pass out in ulNotifiedValue. */
		portMAX_DELAY	 /* Block indefinitely. */
	);

	if ((ulNotifiedValue & 0x01) != 0)
	{
		/* Bit 0 was set */
		// continue forward
	}
	else
	{
		// continue forward anyway. can be handled differently. will expand later.
	}

	while (1)
	{
		//printf("worker_task_a: %d\n", i);

		// send task notification to worker tasks, setup is done.
		/* Set bit 1 in the notification value of the task referenced by xTask1Handle. */
		if (xTaskNotify(xTaskHandle_worker_b, (1UL), eSetBits) == pdPASS)
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
		i += ulDelayInSeconds;
	}
}



#include "esp_partition.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
//////////////////////////////		ARDUINO TASK		//////////////////////////////
void setup()
{
	// note that this does nvs_init() internally!
	initArduino();

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
	configureWiFiSTAMode();
	//////////////////////////////		WIFI ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_WIFIARDUINO

#ifdef SKELETOR_FEATURE_MQTTARDUINO
	//////////////////////////////		MQTT ARDUINO		//////////////////////////////
	configureMqtt();
	//////////////////////////////		MQTT ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_MQTTARDUINO

#ifdef SKELETOR_FEATURE_LORA
	//////////////////////////////		LORA ARDUINO		//////////////////////////////
	configureLoRa();
	//////////////////////////////		LORA ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_LORA


#ifdef SKELETOR_FEATURE_SPIFFSARDUINO
	if (!SPIFFS.begin(false))
	{
		Serial.println("SPIFFS Mount Failed");
		return;
	}
	else
	{
		Serial.println("SPIFFS Mount OK.");
	}	
	listDir(SPIFFS, "/", 0);
//	writeFile(SPIFFS, "/hello.txt", "Hello ");
//	appendFile(SPIFFS, "/hello.txt", "World!\r\n");
//	readFile(SPIFFS, "/hello.txt");
//	renameFile(SPIFFS, "/hello.txt", "/foo.txt");
//	readFile(SPIFFS, "/foo.txt");
//	deleteFile(SPIFFS, "/foo.txt");
//	testFileIO(SPIFFS, "/test.txt");
//	deleteFile(SPIFFS, "/test.txt");



// esp idf spiffs functionality:
// esp_vfs_spiffs_conf_t conf = {
// 	.base_path = "/spiffs",
// 	.partition_label = NULL,
// 	.max_files = 5,
// 	.format_if_mount_failed = false
// };

// esp_err_t err = esp_vfs_spiffs_register(&conf);
// if(err == ESP_OK)
// {
// 	printf("filesystem registered successfully.\n");

// 	size_t total_bytes = 0;
// 	size_t used_bytes = 0;
// 	err = esp_spiffs_info(NULL, &total_bytes, &used_bytes);
// 	if(err == ESP_OK)
// 	{
// 		printf("total: %u bytes, filled: %u bytes\n", total_bytes, used_bytes);
// 	}
// }
#endif // #ifdef SKELETOR_FEATURE_SPIFFSARDUINO

	//	size_t ul;
	//	esp_partition_iterator_t _mypartiterator;
	//	const esp_partition_t *_mypart;
	//	ul = spi_flash_get_chip_size(); Serial.print("Flash chip size: "); Serial.println(ul);
	//	Serial.println("Partiton table:");
	//	_mypartiterator = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
	//	if (_mypartiterator) {
	//	do {
	//	_mypart = esp_partition_get(_mypartiterator);
	//	printf("%x - %x - %x - %x - %s - %i\r\n", _mypart->type, _mypart->subtype, _mypart->address, _mypart->size, _mypart->label, _mypart->encrypted);
	//	} while ((_mypartiterator = esp_partition_next(_mypartiterator)));
	//	}
	//	esp_partition_iterator_release(_mypartiterator);
	//	_mypartiterator = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
	//	if (_mypartiterator) {
	//	do {
	//	_mypart = esp_partition_get(_mypartiterator);
	//	printf("%x - %x - %x - %x - %s - %i\r\n", _mypart->type, _mypart->subtype, _mypart->address, _mypart->size, _mypart->label, _mypart->encrypted);
	//	} while ((_mypartiterator = esp_partition_next(_mypartiterator)));
	//	}
	//	esp_partition_iterator_release(_mypartiterator);


	// send task notification to worker task a, initial setup is done.
	/* Set bit 1 in the notification value of the task referenced by xTaskHandle_worker_a. */
	if (xTaskNotify(xTaskHandle_worker_a, (1UL), eSetBits) == pdPASS)
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

#ifdef SKELETOR_FEATURE_LORA_RECEIVER
	//////////////////////////////		LORA ARDUINO		//////////////////////////////
	pollLoRaReceive();
	//////////////////////////////		LORA ARDUINO		//////////////////////////////
#endif // #ifdef SKELETOR_FEATURE_LORA_RECEIVER
}

// this is from the standard arduino freertos task template, no need to change.
bool ArduinoTaskWDTEnabled = true;
void arduinoTask(void *pvParameters)
{
	setup();

	for (;;)
	{
		if (ArduinoTaskWDTEnabled)
		{
			esp_task_wdt_reset();
		}
		micros(); // update overflow - required?
		loop();
	}
}
//////////////////////////////		ARDUINO TASK		//////////////////////////////


//////////////////////////////		ENTRY POINT		//////////////////////////////
extern "C" void app_main()
{
	// If Arduino Mode is enabled, then initArduino() takes care of nvs init.
	// if we are not using Arduino features, this is required.
	// esp_err_t ret;
	// ret = nvs_flash_init();
	// if(ret != ESP_OK)
	// {
	// 	printf("nvs_flash_init FAILED! error code = [0x%X]\n",ret);
	// }
	// else
	// {
	// 	printf("nvs_flash_init ok.\n");
	// }

	xTaskCreatePinnedToCore(worker_task_a, "worker_task_a", 2048, NULL, 1, NULL, APP_TASKS_RUNNING_CORE);
	xTaskCreatePinnedToCore(worker_task_b, "worker_task_b", 2048, NULL, 1, NULL, APP_TASKS_RUNNING_CORE);

	ArduinoTaskWDTEnabled = true;
	xTaskCreatePinnedToCore(arduinoTask, "arduinoTask", 8192, NULL, 1, NULL, APP_TASKS_RUNNING_CORE);
}
//////////////////////////////		ENTRY POINT		//////////////////////////////
