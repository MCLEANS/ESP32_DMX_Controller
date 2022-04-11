#include <Arduino.h>
#include <WS2812FX.h>
#include <FS.h>   // Include the SPIFFS library

#define LED_COUNT 17
#define LED_PIN 5

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

struct Led_config{
    uint8_t segment_index;
    uint16_t start_index;
    uint16_t stop_index;
    uint8_t mode;
    uint32_t color;
    uint16_t speed;
    bool reverse;
    uint8_t brightness;
};

void configure_leds(Led_config config){
  ws2812fx.setBrightness(config.brightness);

  ws2812fx.setSegment(config.segment_index,
                        config.start_index, 
                        config.stop_index,
                        config.mode, 
                        config.color, 
                        config.speed, 
                        config.reverse);
}




void setup() {
  Led_config configuration = {
  0,
  0,
  LED_COUNT - 1,
  FX_MODE_CHASE_COLOR,
  BLUE,
  4000,
  false,
  255,
};

  ws2812fx.init();
  configure_leds(configuration);
  ws2812fx.start();
}

void loop() {
  ws2812fx.service();
}