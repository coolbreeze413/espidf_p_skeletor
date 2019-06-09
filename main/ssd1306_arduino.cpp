/*
 * ssd1306_arduino.cpp
 *
 *  Created on: Nov 13, 2017
 *      Author: krish
 */

#include "skeletor_config.h"

#ifdef SKELETOR_FEATURE_SSD1306ARDUINO

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
#ifdef SKELETOR_BOARD_WEMOS_OLED
SSD1306  display(0x3c, 5, 4);
#endif // SKELETOR_BOARD_WEMOS_OLED

#ifdef SKELETOR_BOARD_TTGO_LORA32
SSD1306  display(0x3c, 4, 15); // for lora32
#endif // SKELETOR_BOARD_TTGO_LORA32

// SH1106 display(0x3c, D3, D5);


void configureSSD1306Display()
{

#ifdef SKELETOR_BOARD_TTGO_LORA32
	pinMode(16, OUTPUT);
	digitalWrite(16, LOW);
	delay(50);
	digitalWrite(16, HIGH);

	pinMode(25,OUTPUT);
#endif // #ifdef SKELETOR_BOARD_TTGO_LORA32

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
}


void displayCount(uint32_t count)
{
	display.setColor(BLACK);
	display.fillRect(0, 42, 128, 20); // clear the area.
	display.setColor(WHITE);
	display.drawString(0,42, String(count));
	display.display();
}

void displayExternalIPAddress(String ipAddress)
{
	display.setColor(BLACK);
	display.fillRect(0, 0, 128, 20); // clear the area.
	display.setColor(WHITE);
	display.drawString(0,0, ipAddress);
	display.display();
}

// Sender: packetStats -> "TX: N"
// Receiver: packetStats -> "RX: x y" x->rssi, y->snr
void displayLoRaPacket(String packet)
{
	display.setColor(BLACK);
	display.fillRect(0, 0, 128, 20); // clear the area.
	display.setColor(WHITE);
	display.drawString(0,0, packet);
	display.display();
}

#endif // SKELETOR_FEATURE_SSD1306ARDUINO
