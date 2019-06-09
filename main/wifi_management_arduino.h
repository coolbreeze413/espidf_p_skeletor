/*
 * wifi_management_arduino.h
 *
 *  Created on: Nov 13, 2017
 *      Author: krish
 */

#ifndef MAIN_WIFI_MANAGEMENT_ARDUINO_H_
#define MAIN_WIFI_MANAGEMENT_ARDUINO_H_

#include "Arduino.h"
#include <WiFi.h>


#define WIFI_STA_SSID 				"moya"
#define WIFI_STA_PASSWORD 			"leviathanstarburst"

#define WIFI_AP_SSID				"skeletor"
#define WIFI_AP_PASSWORD			"skeletor"


void onWiFiEventCallback(system_event_id_t event, system_event_info_t info);


void configureWiFiSTAMode();
void configureWiFiAPMode();


void printIPv4Address(uint32_t addr);


#endif /* MAIN_WIFI_MANAGEMENT_ARDUINO_H_ */
