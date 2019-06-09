/*
 * lora_arduino.cpp
 *
 *  Created on: Jan 30, 2018
 *      Author: krish
 */

#include "skeletor_config.h"

#ifdef SKELETOR_FEATURE_LORA

#include <SPI.h>
#include <LoRa.h>
#include "ssd1306_arduino.h"

#define SCK     5    	// GPIO5  -- SX1278's SCK
#define MISO    19   	// GPIO19 -- SX1278's MISO
#define MOSI    27   	// GPIO27 -- SX1278's MOSI
#define SS      18   	// GPIO18 -- SX1278's CS
#define RST     14   	// GPIO14 -- SX1278's RESET
#define DI0     26   	// GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    866E6 	// frequency - frequency in Hz (433E6, 866E6, 915E6) etc.


// http://www.semtech.com/images/datasheet/sx1276.pdf
void configureLoRa()
{
	SPI.begin(SCK,MISO,MOSI,SS);

	LoRa.setPins(SS,RST,DI0);

	configASSERT(LoRa.begin(BAND));

	/*
	Spreading factor affects how far apart the radio's transmissions
	are, across the available bandwidth. Radios with different spreading
	factors will not receive each other's transmissions. This is one way you
	can filter out radios you want to ignore, without making an addressing scheme.
	Spreading factor affects reliability of transmission at high rates, however,
	so avoid a huge spreading factor when you're sending continually.
	*/
	LoRa.setSpreadingFactor(8); // ranges from 6-12, default 7 see API docs

	/*
	Sync Word is basically the radio's network ID. Radios with different
	Sync Words will not receive each other's transmissions. This is one way you
	can filter out radios you want to ignore, without making an addressing scheme.
	*/
	LoRa.setSyncWord(0xF3); // ranges from 0-0xFF, default 0x34, see API docs

	Serial.println("\n--------");
	LoRa.dumpRegisters(Serial);
	Serial.println("--------\n");
}


void pollLoRaReceive()
{
	// try to parse packet
	int packetSize = LoRa.parsePacket();
	char c;

	if (packetSize)
	{
		String packet = "";

		// received a packet
		Serial.print("Received packet '");

		// read packet
		while (LoRa.available())
		{
			c = (char)LoRa.read();
			Serial.print(c);
			packet += c;
		}

		// print RSSI of packet
		Serial.print("' with RSSI ");
		Serial.print(LoRa.packetRssi());
		Serial.print(" and SNR ");
		Serial.println(LoRa.packetSnr());

		packet += "," + String(LoRa.packetRssi()) + "," + String(LoRa.packetSnr());

		displayLoRaPacket(packet);

#ifdef USERLED1_PIN
		digitalWrite(USERLED1_PIN, HIGH);   	// turn the LED on (HIGH is the voltage level)
		delay(1000);                			// wait for a second
		digitalWrite(USERLED1_PIN, LOW); 		// turn the LED off by making the voltage LOW
#endif // #ifdef USERLED1_PIN
	}
}


int counter = 0;
void sendLoRaPacket()
{
	Serial.print("Sending packet: ");
	Serial.println(counter);

	displayLoRaPacket(String(counter));

	// send packet
	LoRa.beginPacket();
	//LoRa.print("S");
	LoRa.print(counter);
	LoRa.endPacket();

	counter++;

#ifdef USERLED1_PIN
	digitalWrite(USERLED1_PIN, HIGH);   	// turn the LED on (HIGH is the voltage level)
	delay(1000);                			// wait for a second
	digitalWrite(USERLED1_PIN, LOW); 		// turn the LED off by making the voltage LOW
#endif // #ifdef USERLED1_PIN
}

#endif // #ifdef SKELETOR_FEATURE_LORA
