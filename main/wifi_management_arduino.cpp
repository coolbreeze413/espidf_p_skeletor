/*
 * wifi_management_arduino.cpp
 *
 *  Created on: Nov 13, 2017
 *      Author: krish
 */


#include <WiFi.h>
#include "wifi_management_arduino.h"

// dependencies:
#include "ssd1306_arduino.h"

// triggers, dependent on us:
#include "mqtt_arduino.h"


ip4_addr_t ip_addr;
ip4_addr_t netmask;
ip4_addr_t gw;



void printIPv4Address(uint32_t addr)
{
	unsigned char bytes[4];

	bytes[0] = addr & 0xFF;
	bytes[1] = (addr >> 8) & 0xFF;
	bytes[2] = (addr >> 16) & 0xFF;
	bytes[3] = (addr >> 24) & 0xFF;

	printf("[%d.%d.%d.%d]\n", bytes[0], bytes[1], bytes[1], bytes[3]);
}


void configureWiFiSTAMode()
{
	WiFi.enableSTA(true);
	WiFi.onEvent(onWiFiEventCallback, SYSTEM_EVENT_STA_GOT_IP);
	WiFi.onEvent(onWiFiEventCallback, SYSTEM_EVENT_STA_DISCONNECTED);

	printf("Initiating WiFi STA connection to [%s].\n", WIFI_STA_SSID);
	WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);
}


void configureWiFiAPMode()
{
	WiFi.enableAP(true);
	WiFi.onEvent(onWiFiEventCallback, SYSTEM_EVENT_AP_START);
	WiFi.onEvent(onWiFiEventCallback, SYSTEM_EVENT_AP_STOP);
	WiFi.onEvent(onWiFiEventCallback, SYSTEM_EVENT_AP_STACONNECTED);
	WiFi.onEvent(onWiFiEventCallback, SYSTEM_EVENT_AP_STADISCONNECTED);

	printf("Initiating WiFi AP with SSID [%s].\n", WIFI_AP_SSID);

	WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);
	WiFi.softAPsetHostname("s.k.e.l.e.t.o.r");
	// TODO the AP Config below does not work. need to debug, later.
	//WiFi.softAPConfig(IPAddress((const uint8_t *)"13.0.0.0"), IPAddress((const uint8_t *)"13.0.0.0"), IPAddress((const uint8_t *)"255.255.255.0"));
}


void onWiFiEventCallback(system_event_id_t event, system_event_info_t info)
{
	// we are interested in certain events provided by the WiFi subsystem:
	// SYSTEM_EVENT_STA_GOT_IP shows that we are connected to the router in STA mode.
	// SYSTEM_EVENT_STA_DISCONNECTED shows that we are disconnected from the router.
	switch(event)
	{
		// STA events:
		case SYSTEM_EVENT_STA_GOT_IP:
		{
			// get the IP Address info:
			tcpip_adapter_ip_info_t ip_info = ((system_event_sta_got_ip_t)info.got_ip).ip_info;

			ip_addr = ip_info.ip;
			netmask = ip_info.netmask;
			gw = ip_info.gw;

			printf("\ngot IP address from router\n");
			printf("ip:");printIPv4Address(ip_addr.addr);
			printf("nm:");printIPv4Address(netmask.addr);
			printf("gw:");printIPv4Address(gw.addr);
			printf("\n");

			// arduino utility class IPAddress which takes: u32, u8 array, char*
			// and can be converted into any of these as well!
			// IPAddress(ip_addr.addr).toString();
			// this can also be used.

			connectToMqtt();
		}
		break;

		case SYSTEM_EVENT_STA_DISCONNECTED:
		{
			printf("sta disconnected from router\n");
		}
		break;


		// AP Events:
		case SYSTEM_EVENT_AP_START:
		{
			printf("ap [%s] started, interface IP: [%s]\n", WIFI_AP_SSID, WiFi.softAPIP().toString().c_str());

			displayCount(WiFi.softAPgetStationNum());
		}
		break;

		case SYSTEM_EVENT_AP_STOP:
		{
			printf("ap [%s] stopped\n", WIFI_AP_SSID);
		}
		break;

		case SYSTEM_EVENT_AP_STACONNECTED:
		{
			system_event_ap_staconnected_t sta_info = (system_event_ap_staconnected_t)info.sta_connected;
			printf("sta connected 	 : AID: [%u], MAC ID: %02X:%02X:%02X:%02X:%02X:%02X\n",
					sta_info.aid,
					sta_info.mac[0],
					sta_info.mac[1],
					sta_info.mac[2],
					sta_info.mac[3],
					sta_info.mac[4],
					sta_info.mac[5]);

			displayCount(WiFi.softAPgetStationNum());
		}
		break;

		case SYSTEM_EVENT_AP_STADISCONNECTED:
		{
			system_event_ap_stadisconnected_t sta_info = (system_event_ap_stadisconnected_t)info.sta_disconnected;
			printf("sta disconnected : AID: [%u], MAC ID: %02X:%02X:%02X:%02X:%02X:%02X\n",
					sta_info.aid,
					sta_info.mac[0],
					sta_info.mac[1],
					sta_info.mac[2],
					sta_info.mac[3],
					sta_info.mac[4],
					sta_info.mac[5]);

			displayCount(WiFi.softAPgetStationNum());
		}
		break;

		default:
		{
			printf("unhandled event: %u\n", event);
		}
		break;

	}
}

