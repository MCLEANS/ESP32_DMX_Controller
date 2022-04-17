#ifndef DEFINES_H
#define DEFINES_H

#define LEN_WLAN_SSID 35
#define LEN_WLAN_PWD 65

#define WIFI_CONFIG_TIMEOUT 600000

/* Put your SSID & Password */
const char AP_ssid[] = "ESP32_DMX";  // Enter SSID here
const char AP_password[] = "";  //Enter Password here

char STA_SSID[] = "jack_mcleans";  // Enter SSID here
char STA_PASSWORD[] = "12345678";  //Enter Password here

#define LED_COUNT 17
#define LED_PIN 5

#define MAX_FILE_SIZE 4096

#endif //DEFINES_H