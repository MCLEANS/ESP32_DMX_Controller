#include <ws2812.h>

WS2812::WS2812(uint16_t num_leds, uint8_t pin, neoPixelType type):WS2812FX(num_leds,
                                                                            pin,
                                                                            type){

       
}


void WS2812::configure(WS2812_config config){
    setBrightness(config.brightness);

    setSegment(config.segment_index,
                        config.start_index, 
                        config.stop_index,
                        config.mode, 
                        config.color, 
                        config.speed, 
                        config.reverse);
}

WS2812::~WS2812(){

}