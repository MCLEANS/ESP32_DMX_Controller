#include <UI.h>

UI::UI(){

}

void UI::start_html_page(WebServer& server, String& page_content, const String& title, const String& esp_chipid, String address) {
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
	s.replace("{mac}", address);

	page_content += s;
}

void UI::set_color_picker(String& page_content, WS2812_config &ws2812_config){
  RESERVE_STRING(s, LARGE_STR);
  s = FPSTR(WEB_PAGE_COLOR_PICKER);

  String hex_color = "#";
  hex_color.concat(String(ws2812_config.color,HEX));
  s.replace("{led_color}",hex_color);

  page_content += s;

}

void UI::end_html_page(WebServer& server, String& page_content) {
	if (page_content.length()) {
		server.sendContent(page_content);
	}
	server.sendContent_P(WEB_PAGE_FOOTER);
}

UI::~UI(){

}
