#ifndef _FILE_HANDLER_H
#define _FILE_HANDLER_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ws2812.h>

#define MAX_FILE_SIZE 4096

#define LEN_WLAN_SSID 35
#define LEN_WLAN_PWD 65

const char filename[] = "/config.json";

class File_handler{
    private:
    private:
    public:
    public:
        File_handler();
        bool save(WS2812_config &ws2812_config);
        bool load(WS2812_config &ws2812_config);
        ~File_handler();
};

#endif //_FILE_HANDLER_H