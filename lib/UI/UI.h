#ifndef _UI_H
#define _UI_H

#include <Arduino.h>
#include <html_content.h>
#include <WebServer.h>

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
        UI();
        void start_html_page(WebServer& server,String& page_content, const String& title, const String& esp_chipid, String address);
        void set_color_picker(String& page_content);
        void end_html_page(WebServer& server,String& page_content);
        ~UI();
    public:
};

#endif //_UI_H