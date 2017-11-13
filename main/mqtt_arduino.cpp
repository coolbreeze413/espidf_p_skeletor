/*
 * mqtt_arduino.cpp
 *
 *  Created on: Nov 13, 2017
 *      Author: krish
 */

#include "Arduino.h"
#include <AsyncMqttClient.h>
#include <WiFi.h>

#include "mqtt_arduino.h"

// dependencies:
#include "external_ip_detect_arduino.h"

AsyncMqttClient mqttClient;

String mqttPublishIPAddress;

void publishIPAddress()
{
	mqttPublishIPAddress = "ip: ";
	mqttPublishIPAddress += externalIP;
	mqttPublishIPAddress += ", updated: ";
	mqttPublishIPAddress += lastUpdate;

	mqttClient.publish(SKELETOR_MQTT_TOPIC_IPADDR, 2, true, mqttPublishIPAddress.c_str());
}


void publishAliveStatus()
{
	mqttClient.publish(SKELETOR_MQTT_TOPIC_STATUS, 2, true, SKELETOR_MQTT_STATUS_ALIVE);
}


void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}


void subscribeToCommands()
{
	uint16_t packetIdSub = mqttClient.subscribe(SKELETOR_MQTT_TOPIC_COMMAND, 2);
	//Serial.print("Subscribing at QoS 2, packetId: ");
	//Serial.println(packetIdSub);
}

void onMqttConnect(bool sessionPresent)
{
	  printf("** Connected to the broker **");

	  publishAliveStatus();

	  subscribeToCommands();
}


void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
	printf("Disconnected from MQTT.");

	if (reason == AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT)
	{
		printf("Bad server fingerprint.");
	}

	if (WiFi.isConnected())
	{
		printf("Reconnecting to MQTT...");
		mqttClient.connect();
	}
}


void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}


void onMqttUnsubscribe(uint16_t packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}


void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  payload: ");
  Serial.println(payload);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  //Serial.print("  len: ");
  //Serial.println(len);
  //Serial.print("  index: ");
  //Serial.println(index);
  //Serial.print("  total: ");
  //Serial.println(total);

  String topic_str(topic);
  String message_str(payload);

  if(topic_str.equals(SKELETOR_MQTT_TOPIC_COMMAND) && message_str.equals(SKELETOR_MQTT_COMMAND_REFRESH))
  {
	  detectExternalIP(); // force ip address update
	  publishIPAddress(); // force publish
  }
}


void onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}


void configureMqtt()
{
	mqttClient.onConnect(onMqttConnect);

	mqttClient.onDisconnect(onMqttDisconnect);

	mqttClient.onSubscribe(onMqttSubscribe);

	mqttClient.onUnsubscribe(onMqttUnsubscribe);

	mqttClient.onMessage(onMqttMessage);

	mqttClient.onPublish(onMqttPublish);

	mqttClient.setServer(SKELETOR_MQTT_BROKER_HOST, SKELETOR_MQTT_BROKER_PORT);

	mqttClient.setClientId(SKELETOR_MQTT_CLIENTID);

	mqttClient.setKeepAlive((uint16_t)SKELETOR_MQTT_KEEP_ALIVE_TIME_SEC); // 5 minute keep alive.

	mqttClient.setWill(SKELETOR_MQTT_TOPIC_STATUS, 2, true, SKELETOR_MQTT_STATUS_DEAD);
}
