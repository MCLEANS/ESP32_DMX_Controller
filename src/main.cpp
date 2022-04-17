#include <Arduino.h>
#include <WS2812FX.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DNSServer.h>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include <string.h>

#include "defines.h"
#include "html_content.h"

/******************************************************************
 * String Constants                                               *
 ******************************************************************/
constexpr unsigned SMALL_STR = 64-1;
constexpr unsigned MED_STR = 256-1;
constexpr unsigned LARGE_STR = 512-1;
constexpr unsigned XLARGE_STR = 1024-1;

#define RESERVE_STRING(name, size) String name((const char*)nullptr); name.reserve(size)

String esp_chipid;

#define LEN_WLANSSID 35				// credentials for wifi connection

struct struct_wifiInfo {
	char ssid[LEN_WLANSSID];
	uint8_t encryptionType;
	int32_t RSSI;
	int32_t channel;
};

struct struct_wifiInfo *wifiInfo;
uint8_t count_wifiInfo;


WebServer server(80);
DNSServer dnsServer;

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

Led_config ws2812_config = {
  0,
  0,
  LED_COUNT - 1,
  FX_MODE_BICOLOR_CHASE,
  BLUE,
  4000,
  false,
  255,
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

bool save_config(){
  StaticJsonDocument<200> doc;
  //doc["wifi_credentials"]["ssid"] = wifi_ssid;
  //doc["wifi_credentials"]["password"] = wifi_password;
  doc["ws2812"]["segment_index"] = ws2812_config.segment_index;
  doc["ws2812"]["start_index"] = ws2812_config.start_index;
  doc["ws2812"]["stop_index"] = ws2812_config.stop_index;
  doc["ws2812"]["mode"] = ws2812_config.mode;
  doc["ws2812"]["color"] = ws2812_config.color;
  doc["ws2812"]["speed"] = ws2812_config.speed;
  doc["ws2812"]["reverse"] = ws2812_config.reverse;
  doc["ws2812"]["brightness"] = ws2812_config.brightness;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  serializeJson(doc, configFile);
  return true;
}

bool load_config(){
  File configFile = SPIFFS.open("/config.json", "r");
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
  Serial.print("Configuration File : ");
  Serial.println(ws2812_configurations);

  return true;
}

/*****************************************************************
 * html helper functions                                         *
 *****************************************************************/

static void start_html_page(String& page_content, const String& title) {
	RESERVE_STRING(s, LARGE_STR);
	s = FPSTR(WEB_PAGE_HEADER);
	s.replace("{t}", title);
	server.setContentLength(CONTENT_LENGTH_UNKNOWN);
	server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), s);

	server.sendContent_P(WEB_PAGE_HEADER_HEAD);

	s = FPSTR(WEB_PAGE_HEADER_BODY);
	s.replace("{t}", title);
	if (title != " ") {
		s.replace("{n}", F("&raquo;"));
	} else {
		s.replace("{n}", emptyString);
	}
	s.replace("{id}", esp_chipid);
	s.replace("{mac}", WiFi.macAddress());

	page_content += s;
}

static void set_color_picker(String& page_content){
  RESERVE_STRING(s, LARGE_STR);
  s = FPSTR(WEB_PAGE_COLOR_PICKER);

  String hex_color = "#";
  hex_color.concat(String(ws2812_config.color,HEX));
  s.replace("{led_color}",hex_color);

  page_content += s;

}

static void end_html_page(String& page_content) {
	if (page_content.length()) {
		server.sendContent(page_content);
	}
	server.sendContent_P(WEB_PAGE_FOOTER);
}

static void sendHttpRedirect() {
	server.sendHeader(F("Location"), F("http://192.168.4.1/"));
	server.send(302, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), emptyString);
}


/*****************************************************************
 * Webserver root: show all options                              *
 *****************************************************************/
static void webserver_root() {
  RESERVE_STRING(page_content, XLARGE_STR);
  start_html_page(page_content, emptyString);	
  server.sendContent(page_content);
  page_content = emptyString;

  set_color_picker(page_content);
  server.sendContent(page_content);
  page_content = emptyString;

  end_html_page(page_content);
}

static void webserver_not_found() {
	if (WiFi.status() != WL_CONNECTED) {
		if ((server.uri().indexOf(F("success.html")) != -1) || (server.uri().indexOf(F("detect.html")) != -1)) {
			server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), FPSTR(WEB_IOS_REDIRECT));
		} else {
			sendHttpRedirect();
		}
	} else {
		server.send(404, FPSTR(TXT_CONTENT_TYPE_TEXT_PLAIN), F("Not found."));
	}
}

static void handle_color_picker(){
  if (server.hasArg("color")) {
    Serial.print("Received color : ");
    ws2812_config.color = (uint32_t)strtol(server.arg("color").c_str(), NULL, 16);
    /* Configure ws2812 leds */
    configure_leds(ws2812_config);
    /* Start leds */
    ws2812fx.start();
    save_config();    
  }
  else{
    Serial.println("Unknown Parameter");
  }
}

/*****************************************************************
 * Webserver setup                                               *
 *****************************************************************/
static void setup_webserver() {
	server.on("/", webserver_root);
  server.on("/color_picker", handle_color_picker);
  server.onNotFound(webserver_not_found);
	server.begin();
}

static int selectChannelForAp() {
	std::array<int, 14> channels_rssi;
	std::fill(channels_rssi.begin(), channels_rssi.end(), -100);

	for (unsigned i = 0; i < count_wifiInfo; i++) {
		if (wifiInfo[i].RSSI > channels_rssi[wifiInfo[i].channel]) {
			channels_rssi[wifiInfo[i].channel] = wifiInfo[i].RSSI;
		}
	}

	if ((channels_rssi[1] < channels_rssi[6]) && (channels_rssi[1] < channels_rssi[11])) {
		return 1;
	} else if ((channels_rssi[6] < channels_rssi[1]) && (channels_rssi[6] < channels_rssi[11])) {
		return 6;
	} else {
		return 11;
	}
}

/*****************************************************************
 * WifiConfig                                                    *
 *****************************************************************/
static void wifiConfig() {
	Serial.println("Starting WiFi Access Point");
  Serial.print("AP ID: ");
  Serial.println(AP_ssid);
  Serial.print("Password: ");
  Serial.println(AP_password);

	WiFi.disconnect(true);
	count_wifiInfo = WiFi.scanNetworks(false /* scan async */, true /* show hidden networks */);
	delete [] wifiInfo;
	wifiInfo = new struct_wifiInfo[count_wifiInfo];

	for (int i = 0; i < count_wifiInfo; i++) {
		String SSID;
		uint8_t* BSSID;

		memset(&wifiInfo[i], 0, sizeof(struct_wifiInfo));
		WiFi.getNetworkInfo(i, SSID, wifiInfo[i].encryptionType,
		wifiInfo[i].RSSI, BSSID, wifiInfo[i].channel);
		SSID.toCharArray(wifiInfo[i].ssid, sizeof(wifiInfo[0].ssid));
	}

	WiFi.mode(WIFI_AP);
	const IPAddress apIP(192, 168, 4, 1);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP(AP_ssid, AP_password, selectChannelForAp());
	// In case we create a unique password at first start
	
	// Ensure we don't poison the client DNS cache
	dnsServer.setTTL(0);
	dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
	dnsServer.start(53, "*", apIP);							// 53 is port for DNS server

	setup_webserver();

}

static void waitForWifiToConnect(int maxRetries) {
	int retryCount = 0;
	while ((WiFi.status() != WL_CONNECTED) && (retryCount < maxRetries)) {
		delay(500);
		++retryCount;
	}
}

void setup() {
  Serial.begin(115200);
  /* Mount file system */
  Serial.println("Mounting FS...");
  if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
  }

  /* Read chip-ID */
  uint64_t chipid_num;
	chipid_num = ESP.getEfuseMac();
	esp_chipid = String((uint16_t)(chipid_num >> 32), HEX);
	esp_chipid += String((uint32_t)chipid_num, HEX);

  WiFi.persistent(false);
  wifiConfig();
  /* Read configuration file from flash */
  load_config();

  /* Initialize ws2812 leds */
  ws2812fx.init();

  /* Configure ws2812 leds */
  configure_leds(ws2812_config);

  /* Start leds */
  ws2812fx.start();
}

void loop() {
  ws2812fx.service();
  dnsServer.processNextRequest();
	server.handleClient();
	yield();
  
}