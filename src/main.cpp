#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DNSServer.h>

#include <SPI.h>
#include "Ethernet_Generic.h"
#include "EthernetWebServer-impl.h"
#include "Parsing-impl.h"
#include <EthernetWebServer.hpp>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include <string.h>

#include "defines.h"
#include "file_handler.h"
#include "html_content.h"
#include "UI.h"
#include "ws2812.h"

#define USE_THIS_SS_PIN   5 // For ESP32
#define NUMBER_OF_MAC     20


byte mac[][NUMBER_OF_MAC] =
{
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14 },
};

String esp_chipid;
struct struct_wifiInfo {
	char ssid[LEN_WLAN_SSID];
	uint8_t encryptionType;
	int32_t RSSI;
	int32_t channel;
};

struct struct_wifiInfo *wifiInfo;
uint8_t count_wifiInfo;

WIFI_credentials wifi_credentials;
File_handler config_file;
UI ui;

WebServer server_wifi(80);
EthernetWebServer server_eth(80); 
DNSServer dnsServer;
WS2812 ws2812fx(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

WS2812_config ws2812_config = {
  0,
  0,
  LED_COUNT - 1,
  FX_MODE_BICOLOR_CHASE,
  BLUE,
  4000,
  false,
  255,
};

static void sendHttpRedirect() {
	if(ui.is_ethernet_enabled){
		server_eth.sendHeader(F("Location"), F("http://192.168.4.1/"));
		server_eth.send(302, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), emptyString);
	}
	else{
		server_wifi.sendHeader(F("Location"), F("http://192.168.4.1/"));
		server_wifi.send(302, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), emptyString);
	}
	
}

/*****************************************************************
 * Webserver root: show all options                              *
 *****************************************************************/
static void webserver_control() {
  RESERVE_STRING(page_content, XLARGE_STR);
  ui.start_html_page(server_eth,server_wifi, page_content, emptyString, esp_chipid, WiFi.macAddress());	

  if(ui.is_ethernet_enabled)server_eth.sendContent(page_content);
  else server_wifi.sendContent(page_content);

  page_content = emptyString;

  ui.set_color_picker(page_content, ws2812_config);

  if(ui.is_ethernet_enabled)server_eth.sendContent(page_content);
  else server_wifi.sendContent(page_content);

  page_content = emptyString;

  ui.end_html_page(server_eth,server_wifi,page_content);
}

static void webserver_not_found() {
	if (WiFi.status() != WL_CONNECTED) {
		if ((server_wifi.uri().indexOf(F("success.html")) != -1) || (server_wifi.uri().indexOf(F("detect.html")) != -1)) {
			server_wifi.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), FPSTR(WEB_IOS_REDIRECT));
		} else {
			sendHttpRedirect();
		}
	} else {
		server_wifi.send(404, FPSTR(TXT_CONTENT_TYPE_TEXT_PLAIN), F("Not found."));
	}
}

static void handle_color_picker(){
  if (server_wifi.hasArg("color") || server_eth.hasArg("color")) {
    Serial.print("Color Updated to : ");
	if(ui.is_ethernet_enabled){
		Serial.println(server_eth.arg("color"));
		ws2812_config.color = (uint32_t)strtol(server_eth.arg("color").c_str(), NULL, 16);
	}
	else{
		Serial.println(server_wifi.arg("color"));
		ws2812_config.color = (uint32_t)strtol(server_wifi.arg("color").c_str(), NULL, 16);
	}
    
    /* Configure ws2812 leds */
    ws2812fx.configure(ws2812_config);
    /* Start leds */
    ws2812fx.start();
    /* Save updated configuration to config file */
    config_file.save(ws2812_config, wifi_credentials); 
  }
  else{
    Serial.println("Unknown Parameter");
  }
}

static void setup_webserver() {
	if(ui.is_ethernet_enabled){
		server_eth.on("/", webserver_control);
		server_eth.on("/color_picker", handle_color_picker);
		server_eth.onNotFound(webserver_not_found);
		server_eth.begin();
	}
	else{
		server_wifi.on("/", webserver_control);
		server_wifi.on("/color_picker", handle_color_picker);
		server_wifi.onNotFound(webserver_not_found);
		server_wifi.begin();
	}
	
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

  unsigned long wifi_config_start_time = millis();
	while ((millis() - wifi_config_start_time) < WIFI_CONFIG_TIMEOUT + 500) {
		dnsServer.processNextRequest();
		server_wifi.handleClient();
		yield();
	}

  WiFi.softAPdisconnect(true);
	WiFi.mode(WIFI_STA);

	dnsServer.stop();
	delay(100);

	Serial.print("Connecting to ");
  Serial.println(wifi_credentials.wifi_ssid);

	WiFi.begin(wifi_credentials.wifi_ssid, wifi_credentials.wifi_password);
}

static void waitForWifiToConnect(int maxRetries) {
	int retryCount = 0;
	while ((WiFi.status() != WL_CONNECTED) && (retryCount < maxRetries)) {
		delay(500);
    	Serial.print(".");
		++retryCount;
	}
}

void init_wifi_credentials(String chip_id){
  strcpy(wifi_credentials.wifi_ssid, STA_SSID);
  strcpy(wifi_credentials.wifi_password,STA_PASSWORD);
}

/*****************************************************************
 * WiFi auto connecting script                                   *
 *****************************************************************/
static void connectWifi() {
	if (WiFi.getAutoConnect()) {
		WiFi.setAutoConnect(false);
	}
	if (!WiFi.getAutoReconnect()) {
		WiFi.setAutoReconnect(true);
	}
	WiFi.mode(WIFI_STA);
	WiFi.hostname(AP_ssid);
	WiFi.begin(wifi_credentials.wifi_ssid, wifi_credentials.wifi_password); // Start WiFI

  Serial.print("Connecting to ");
  Serial.print(wifi_credentials.wifi_ssid);
  Serial.print(" Password : ");
  Serial.println(wifi_credentials.wifi_password);

	waitForWifiToConnect(40);
	if (WiFi.status() != WL_CONNECTED) {
		String fss(AP_ssid);
		wifiConfig();
		if (WiFi.status() != WL_CONNECTED) {
			waitForWifiToConnect(20);
      Serial.println("");
		}
	}

  Serial.print("WiFi connected, IP is: ");
  Serial.println(WiFi.localIP().toString());
  //last_signal_strength = WiFi.RSSI();

  if (MDNS.begin(AP_ssid)) {
	MDNS.addService("http", "tcp", 80);
	MDNS.addServiceTxt("http", "tcp", "PATH", "/");
	}
}

void setup() {
  Serial.begin(115200);

  /* Initialize config file */
  config_file.init();

  /* Read chip-ID */
  uint64_t chipid_num;
  chipid_num = ESP.getEfuseMac();
  esp_chipid = String((uint16_t)(chipid_num >> 32), HEX);
  esp_chipid += String((uint32_t)chipid_num, HEX);

  init_wifi_credentials(esp_chipid);

  /* Read configuration file from flash */
  config_file.load(ws2812_config,wifi_credentials);

  WiFi.persistent(false);
  
  Ethernet.init(USE_THIS_SS_PIN); 
  uint16_t index = millis() % NUMBER_OF_MAC;
  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  Ethernet.begin(mac[index]);
  
  if(Ethernet.hardwareStatus() == EthernetNoHardware){
	  Serial.println("No Ethernet found. Stay here forever");
	  ui.is_ethernet_enabled = false;
  }
  
  if (Ethernet.linkStatus() == LinkOFF) {
	  Serial.println("Not connected Ethernet cable");
	  ui.is_ethernet_enabled = false;
  }
  else{
	  Serial.print(F("Using mac index = "));
	  Serial.println(index);
	  Serial.print(F("Connected! IP address: "));
	  Serial.println(Ethernet.localIP());
  }
  
  if(!ui.is_ethernet_enabled){
	  connectWifi();	  
  }
  
  setup_webserver();
 

  /* Setup mDNS */
  if(!MDNS.begin("esp32")) {
     Serial.println("Error starting mDNS");
     return;
	} 

  /* Initialize ws2812 leds */
  ws2812fx.init();
  /* Configure ws2812 leds */
  ws2812fx.configure(ws2812_config);
  /* Start leds */
  ws2812fx.start();
}

void loop() {
  ws2812fx.service();

  if(ui.is_ethernet_enabled){
	  server_eth.handleClient();
  }
  else{
	  server_wifi.handleClient();
  }
  
  yield(); 
}