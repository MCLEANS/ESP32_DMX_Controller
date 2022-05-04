#include "file_handler.h"

File_handler::File_handler(){

}

void File_handler::init(){
    /* Mount file system */
    Serial.println("Mounting FS...");

    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
  }
}

bool File_handler::save(WS2812_config &ws2812_config, WIFI_credentials &credentials){
    Serial.println("Saving configuration file ..");

    StaticJsonDocument<200> doc;
    doc["wifi_credentials"]["ssid"] = credentials.wifi_ssid;
    doc["wifi_credentials"]["password"] = credentials.wifi_password;
    doc["ws2812"]["segment_index"] = ws2812_config.segment_index;
    doc["ws2812"]["start_index"] = ws2812_config.start_index;
    doc["ws2812"]["stop_index"] = ws2812_config.stop_index;
    doc["ws2812"]["mode"] = ws2812_config.mode;
    doc["ws2812"]["color"] = ws2812_config.color;
    doc["ws2812"]["speed"] = ws2812_config.speed;
    doc["ws2812"]["reverse"] = ws2812_config.reverse;
    doc["ws2812"]["brightness"] = ws2812_config.brightness;

    File configFile = SPIFFS.open(filename, "w");
    if (!configFile) {
        Serial.println("Failed to open config file for writing");
        return false;
    }
    serializeJson(doc, configFile);
    return true;
}

bool File_handler::load(WS2812_config &ws2812_config, WIFI_credentials &credentials){
    Serial.println("Loading configuration file ..");

    File configFile = SPIFFS.open(filename, "r");
    if (!configFile) {
        Serial.println("Failed to open config file");
        return false;
    }

    size_t size = configFile.size();
    if (size > MAX_FILE_SIZE) {
        Serial.println("Config file size is too large");
        return false;
    }

    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // We don't use String here because ArduinoJson library requires the input
    // buffer to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    configFile.readBytes(buf.get(), size);

    StaticJsonDocument<200> doc;
    auto error = deserializeJson(doc, buf.get());
    if (error) {
        Serial.println("Failed to parse config file");
        return false;
    }

    strcpy(credentials.wifi_ssid ,doc["wifi_credentials"]["ssid"]);
    strcpy(credentials.wifi_password ,doc["wifi_credentials"]["password"]);
    ws2812_config.segment_index = (uint8_t) doc["ws2812"]["segment_index"];
    ws2812_config.start_index = (uint16_t) doc["ws2812"]["start_index"];
    ws2812_config.stop_index = (uint16_t) doc["ws2812"]["stop_index"];
    ws2812_config.mode = (uint8_t) doc["ws2812"]["mode"];
    ws2812_config.color = (uint32_t) doc["ws2812"]["color"];
    ws2812_config.speed = (uint16_t) doc["ws2812"]["speed"];
    ws2812_config.reverse = (bool) doc["ws2812"]["reverse"];
    ws2812_config.brightness = (uint8_t) doc["ws2812"]["brightness"];

    String ws2812_configurations;
    serializeJson(doc, ws2812_configurations);
    Serial.print("Configuration JSON : ");
    Serial.println(ws2812_configurations);

    return true;
}

File_handler::~File_handler(){

}
