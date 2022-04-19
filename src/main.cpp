#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DNSServer.h>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include <string.h>

#include "file_handler.h"
#include "defines.h"
#include "html_content.h"
#include "UI.h"
#include "ws2812.h"


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

char wifi_ssid[LEN_WLAN_SSID];
char wifi_password[LEN_WLAN_PWD];

File_handler config_file;
UI ui;
WebServer server(80);
DNSServer dnsServer;
WS2812 ws2812fx = WS2812(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

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
	server.sendHeader(F("Location"), F("http://192.168.4.1/"));
	server.send(302, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), emptyString);
}


/*****************************************************************
 * Webserver root: show all options                              *
 *****************************************************************/
static void webserver_root() {
  RESERVE_STRING(page_content, XLARGE_STR);
  ui.start_html_page(server,page_content, emptyString, esp_chipid, WiFi.macAddress());	
  server.sendContent(page_content);
  page_content = emptyString;

  ui.set_color_picker(page_content);
  server.sendContent(page_content);
  page_content = emptyString;

  ui.end_html_page(server,page_content);
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
    ws2812fx.configure(ws2812_config);
    /* Start leds */
    ws2812fx.start();
    config_file.save(ws2812_config); 
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

  unsigned long wifi_config_start_time = millis();
	while ((millis() - wifi_config_start_time) < WIFI_CONFIG_TIMEOUT + 500) {
		dnsServer.processNextRequest();
		server.handleClient();
		yield();
	}

  WiFi.softAPdisconnect(true);
	WiFi.mode(WIFI_STA);

	dnsServer.stop();
	delay(100);

	Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

	WiFi.begin(wifi_ssid, wifi_password);
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
  strcpy(wifi_ssid, STA_SSID);
  strcpy(wifi_password,STA_PASSWORD);
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
	WiFi.begin(wifi_ssid, wifi_password); // Start WiFI

  Serial.print("Connecting to ");
  Serial.print(wifi_ssid);
  Serial.print(" Password : ");
  Serial.println(wifi_password);

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

  init_wifi_credentials(esp_chipid);
  /* Read configuration file from flash */
  config_file.load(ws2812_config);

  WiFi.persistent(false);
  connectWifi();
  setup_webserver();

  /* Initialize ws2812 leds */
  ws2812fx.init();
  /* Configure ws2812 leds */
  ws2812fx.configure(ws2812_config);
  /* Start leds */
  ws2812fx.start();
}

void loop() {
  ws2812fx.service();
  server.handleClient();
  yield(); 
}