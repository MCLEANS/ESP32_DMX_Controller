#ifndef _DEFINES_H
#define _DEFINES_H

/* Ethernet variables and constants */
// Enter a MAC address and IP address for your controller below.

#define MAX_FILE_SIZE 4096

#define LEN_WLAN_SSID 35
#define LEN_WLAN_PWD 65

#define WIFI_CONFIG_TIMEOUT 600000

struct WIFI_credentials{
  char wifi_ssid[LEN_WLAN_SSID];
  char wifi_password[LEN_WLAN_PWD];   
};

/* Put your SSID & Password */
const char AP_ssid[] = "ESP32_DMX";  // Enter SSID here
const char AP_password[] = "";  //Enter Password here

const char STA_SSID[] = "geviton_wireless";  // Enter SSID here
const char STA_PASSWORD[] = "1234ninye#";  //Enter Password here

#define LED_COUNT 15
#define LED_PIN 16

#define MAX_LED_SPEED 12000

#endif //_DEFINES_H