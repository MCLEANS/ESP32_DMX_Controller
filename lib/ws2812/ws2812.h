#ifndef _WS2812_H
#define _WS2812_H

#include <WS2812FX.h>

struct WS2812_config{
    uint8_t segment_index;
    uint16_t start_index;
    uint16_t stop_index;
    uint8_t mode;
    uint32_t color;
    uint16_t speed;
    bool reverse;
    uint8_t brightness;
};

class WS2812 : public WS2812FX{
    private:
    private:
    public:
    public:
        WS2812(uint16_t num_leds, uint8_t pin, neoPixelType type);
        void configure(WS2812_config ws2812_config);
        ~WS2812();

};

#endif //_WS2812_H