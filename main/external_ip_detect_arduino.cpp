/*
 * external_ip_detection.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: krish
 */

#include "skeletor_config.h"

#ifdef SKELETOR_FEATURE_WIFIARDUINO


#include "Arduino.h"
#include "WiFi.h"

// triggers, dependent on us:
#include "mqtt_arduino.h"

const char* host = "api.ipify.org";
const int httpPort = 80;

String httpResponse("");
String temp("");
String externalIP("");
String lastUpdate("");

WiFiClient ipDetectHTTPClient;

String detectExternalIP()
{
	// check if we are connected to WiFi, else return 0.0.0.0
	if(!WiFi.isConnected())
	{
		return String("0.0.0.0");
	}

	// talk to external webserver and detect our wan side ip address.

	//Serial.print("connecting to ");
	//Serial.println(host);

	// Use WiFiClient class to create TCP connections

	if (!ipDetectHTTPClient.connect(host, httpPort))
	{
		Serial.println("connection to host failed");
		return String("0.0.0.0");
	}

	// We now create a URI for the request
	String url = "/";

	//Serial.print("Requesting URL: ");
	//Serial.println(url);

	// This will send the request to the server
	ipDetectHTTPClient.print
	(
		String("GET ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"Connection: close\r\n\r\n"
	);

	unsigned long time_of_start = millis();
	while (ipDetectHTTPClient.available() == 0)
	{
		if (millis() - time_of_start > 5000)
		{
			Serial.println(">>> ipDetectHTTPClient Timeout !");
			ipDetectHTTPClient.stop();
			return String("0.0.0.0");
		}
	}

	// Read all the lines of the reply from server and print them to Serial
	while(ipDetectHTTPClient.available())
	{
		//String line = ipDetectHTTPClient.readStringUntil('\r');
		String line = ipDetectHTTPClient.readString();
		//Serial.print(line);
		httpResponse += line;
	}

	//Serial.println("httpResponse >");
	//Serial.println(httpResponse);
	//Serial.println();
	//Serial.println("closing connection");

	// ip address is between last CR-LF-CR-LF and end of http response.
	int ipBeginningIndex = httpResponse.lastIndexOf("\r\n\r\n") + 4;

	//ipAddress = httpResponse.substring(ipBeginningIndex);
	temp = httpResponse.substring(ipBeginningIndex);

	// Date is between "Date:" and the next CR-LF.
	int dateBeginningIndex = httpResponse.lastIndexOf("Date: ") + 6;
	int dateEndIndex = httpResponse.indexOf("\r\n", dateBeginningIndex) ;

	lastUpdate = httpResponse.substring(dateBeginningIndex, dateEndIndex);

	Serial.print("ipAddress: ");	Serial.println(temp);
	Serial.print("Updated  : ");	Serial.println(lastUpdate);

	if(!externalIP.equals(temp))
	{
		Serial.println("ip address has changed now.");
		externalIP = temp; 			// update
#ifdef SKELETOR_FEATURE_MQTTARDUINO
		publishIPAddress(); 		// publish
#endif // #ifdef SKELETOR_FEATURE_MQTTARDUINO
	}
	else
	{
		Serial.println("ip address has *NOT* changed.");
	}


	//	 for(int j=0; j< httpResponse.length(); j++)
	//	 {
	//		 Serial.print(j);
	//		 Serial.print(" ");
	//		 Serial.println(httpResponse.charAt(j), HEX);
	//	 }

	//	 Serial.println(ipBeginningIndex);


	// clear the http response string object.
	httpResponse = "";

	return externalIP;
}


#endif // SKELETOR_FEATURE_WIFIARDUINO
