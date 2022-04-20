#ifndef _FILE_HANDLER_H
#define _FILE_HANDLER_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ws2812.h>
#include "defines.h"

const char filename[] = "/config.json";

class File_handler{
    private:
    private:
    public:
    public:
        File_handler();
        void init();
        bool save(WS2812_config &ws2812_config, WIFI_credentials &credentials);
        bool load(WS2812_config &ws2812_config, WIFI_credentials &credentials);
        ~File_handler();
};

#endif //_FILE_HANDLER_H