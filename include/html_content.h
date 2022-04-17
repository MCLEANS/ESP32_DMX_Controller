#ifndef HTML_CONTENT_H
#define HTML_CONTENT_H

const char TXT_CONTENT_TYPE_JSON[] PROGMEM = "application/json";
const char TXT_CONTENT_TYPE_INFLUXDB[] PROGMEM = "application/x-www-form-urlencoded";
const char TXT_CONTENT_TYPE_TEXT_HTML[] PROGMEM = "text/html; charset=utf-8";
const char TXT_CONTENT_TYPE_TEXT_PLAIN[] PROGMEM = "text/plain";
const char TXT_CONTENT_TYPE_IMAGE_PNG[] PROGMEM = "image/png";


#define WEB_BOARD_NAME "DMX CONTROLLER"
#define WEB_FIRMWARE  "Firmware Version"
#define WEB_FIRMWARE_VERSION  "DMX_V1.0.0.2022"
#define WEB_HOME  "Home"
#define WEB_BACK_TO_HOME  "Back to Home"


const char WEB_PAGE_HEADER[] PROGMEM = "<!DOCTYPE html><html>\
<head>\
<title>{t}</title>";

const char WEB_PAGE_HEADER_HEAD[] PROGMEM = "<meta name='viewport' content='width=device-width'>\
<style type='text/css'>\
body{font-family:Arial;margin:0}\
.content{margin:10px}\
.r{text-align:right}\
td{vertical-align:top;}\
a{text-decoration:none;padding:10px;background:#3ba;color:white;display:block;width:auto;border-radius:5px;box-shadow:0px 2px 2px #3ba;}\
.wifi{background:none;color:blue;padding:5px;display:inline;}\
input[type='text']{width:100%;}\
input[type='password']{width:100%;}\
input[type='submit']{color:white;text-align:left;border-radius:5px;font-size:medium;background:#b33;box-shadow:0px 2px 2px #b33;padding:9px !important;width:100%;border-style:none;}\
input[type='submit']:hover {background:#d44} \
.s_green{padding:9px !important;width:100%;border-style:none;background:#3bf;color:white;text-align:left;}\
</style>\
</head><body>\
<div id='nav' style='min-height:129px;background-color:#3ba;margin-bottom:20px;box-shadow:0px 4px 6px #3ba'>\
<a href='/' style='background:none;display:inline'><img src='/images?name=luftdaten_logo' style='float:left;margin:20px' width='100' height='89'/></a>";

const char WEB_PAGE_HEADER_BODY[] PROGMEM = "<h3 style='margin:0'>" WEB_BOARD_NAME "</h3>\
<small>ID: {id}<br/>MAC: {mac}<br/>" WEB_FIRMWARE ": " WEB_FIRMWARE_VERSION "<br/>(" __DATE__ " " __TIME__ ")<br/>\
<a href='link to issues page' target='_blank' rel='noreferrer'>Report an issue</a>\
</small></div>\
<div class='content'><h4>" WEB_HOME " {n} {t}";

const char WEB_PAGE_COLOR_PICKER[] PROGMEM = "</h4><label for='colorpicker'>Color Picker:</label><input type='color' id='colorpicker' value='{led_color}'><hr>\
<script>\  
function changeColor(){\
    document.getElementById('colorpicker').addEventListener('change', (e) => {\
        let color_value =  e.target.value;\
        document.getElementById('nav').style.backgroundColor = color_value; \
        console.log(typeof 'color_value');\
        var xhttp = new XMLHttpRequest();\
        let payload_header = 'color_picker?color=';\
        let payload = payload_header.concat(color_value.substring(1));\
        console.log(payload);\
        xhttp.open(\"GET\",payload, true);\
        xhttp.send();\
    });\
};\
changeColor();\
</script>";

const char WEB_PAGE_FOOTER[] PROGMEM = "<br/><br/><a href='/' style='display:inline;'>" WEB_BACK_TO_HOME "</a>"
                "<br/><br/><br/>"
                "</div></body></html>\r\n";

const char WEB_IOS_REDIRECT[] PROGMEM = "<html><body>Redirecting...\
<script type=\"text/javascript\">\
window.location = \"http://192.168.4.1\";\
</script>\
</body></html>";

#endif //HTML_CONTENT_H