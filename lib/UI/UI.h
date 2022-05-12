#ifndef _UI_H_
#define _UI_H_

#include <Arduino.h>
#include <WebServer.h>
#include <EthernetWebServer.hpp>

#include "ws2812.h"

/******************************************************************
 * String Constants                                               *
 ******************************************************************/
constexpr unsigned SMALL_STR = 64-1;
constexpr unsigned MED_STR = 256-1;
constexpr unsigned LARGE_STR = 512-1;
constexpr unsigned XLARGE_STR = 1024-1;

#define RESERVE_STRING(name, size) String name((const char*)nullptr); name.reserve(size)


class UI{
    private:
    private:
    public:
        bool is_ethernet_enabled = true;
    public:
        UI();

        ~UI();
};

#endif //_UI_H_