/*
 * mqtt_arduino.h
 *
 *  Created on: Nov 13, 2017
 *      Author: krish
 */

#ifndef MAIN_MQTT_ARDUINO_H_
#define MAIN_MQTT_ARDUINO_H_

#include "Arduino.h"

#define SKELETOR_MQTT_BROKER_HOST	 				"iot.eclipse.org" // "test.mosquitto.org";
#define SKELETOR_MQTT_BROKER_PORT	 				1883

#define SKELETOR_MQTT_CLIENTID						"skeletor-from-optimuslogic"

#define SKELETOR_MQTT_TOPIC_IPADDR					"optimuslogic/skeletor/ipaddress"
#define SKELETOR_MQTT_TOPIC_COMMAND					"optimuslogic/skeletor/command"
#define SKELETOR_MQTT_TOPIC_STATUS					"optimuslogic/skeletor/status"

#define SKELETOR_MQTT_COMMAND_REFRESH				"refresh"

#define SKELETOR_MQTT_STATUS_ALIVE					"it's alive!"
#define SKELETOR_MQTT_STATUS_DEAD					"dead as a doornail"

// 5 minute keep alive
#define SKELETOR_MQTT_KEEP_ALIVE_TIME_SEC			300



void publishIPAddress();
void connectToMqtt();


void configureMqtt();

#endif /* MAIN_MQTT_ARDUINO_H_ */
