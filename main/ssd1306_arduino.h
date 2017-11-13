/*
 * ssd1306_arduino.h
 *
 *  Created on: Nov 13, 2017
 *      Author: krish
 */

#ifndef MAIN_SSD1306_ARDUINO_H_
#define MAIN_SSD1306_ARDUINO_H_

#include "Arduino.h"

void configureSSD1306Display();

void displayCount(uint32_t count);
void displayExternalIPAddress(String ipAddress);




#endif /* MAIN_SSD1306_ARDUINO_H_ */
