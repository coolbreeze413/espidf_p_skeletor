/*
 * lora_arduino.h
 *
 *  Created on: Jan 30, 2018
 *      Author: krish
 */

#ifndef MAIN_LORA_ARDUINO_H_
#define MAIN_LORA_ARDUINO_H_

#include "skeletor_config.h"

void configureLoRa();

void sendLoRaPacket();
void pollLoRaReceive();



#endif /* MAIN_LORA_ARDUINO_H_ */
