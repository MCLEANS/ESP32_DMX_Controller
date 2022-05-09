#ifndef _HTML_CONTENT_H
#define _HTML_CONTENT_H

#include "css.h"

const char TXT_CONTENT_TYPE_JSON[] PROGMEM = "application/json";
const char TXT_CONTENT_TYPE_INFLUXDB[] PROGMEM = "application/x-www-form-urlencoded";
const char TXT_CONTENT_TYPE_TEXT_HTML[] PROGMEM = "text/html; charset=utf-8";
const char TXT_CONTENT_TYPE_TEXT_PLAIN[] PROGMEM = "text/plain";
const char TXT_CONTENT_TYPE_IMAGE_PNG[] PROGMEM = "image/png";


#define WEB_BOARD_NAME "DMX CONTROLLER"
#define WEB_FIRMWARE  "Firmware Version"
#define WEB_FIRMWARE_VERSION  "DMX_V1.0.0.2022"
#define WEB_HOME  "CONTROL"
#define WEB_BACK_TO_HOME  "Back to Home"


const char WEB_PAGE_HEADER[] PROGMEM = "\
<!DOCTYPE html>\
<html lang='en'>\
    <head>\
        <meta charset='utf-8' />\
        <meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no' />\
        <meta name='description' content='' />\
        <meta name='author' content='' />\
        <title>{t}</title>\
";

const char WEB_PAGE_START_BODY[] PROGMEM = "\
    </head>\
    <body>\
";

const char WEB_PAGE_NAV[] PROGMEM = "\
<!-- Responsive navbar--> \
<nav class='navbar navbar-expand-lg navbar-dark bg-dark'> \
    <div class='container'> \
        <a class='navbar-brand' href='#'>Dmx Controller</a> \
        <button class='navbar-toggler' type='button' data-bs-toggle='collapse' data-bs-target='#navbarSupportedContent' aria-controls='navbarSupportedContent' aria-expanded='false' aria-label='Toggle navigation'> <span class='navbar-toggler-icon'></span></button> \
        <div class='collapse navbar-collapse' id='navbarSupportedContent'> \
            <ul class='navbar-nav ms-auto mb-2 mb-lg-0'> \
                <li class='nav-item'><a class='nav-link active' aria-current='page' href='#!'>Home</a></li> \
                <li class='nav-item'><a class='nav-link' href='#!'>About</a></li> \
                <li class='nav-item'><a class='nav-link' href='#!'>Contact</a></li> \
            </ul> \
        </div> \
    </div> \
</nav> \
";

/*************************** CONTROL PAGE START *************************************************/
const char WEB_PAGE_COLOR_PICKER[] PROGMEM = " \
<!-- Color Picker Content section--> \
<section id = 'color_picker' class='py-5'> \
    <div class='container my-5'> \
        <div class='jumbotron'> \
            <div class='row  justify-content-center'> \
                <div style='text-align: center;' class='col-lg-6'> \
                    <button for='colorpicker' style='width: 50%;' class='button button_picker'> <h6>Color Picker</h6> <input style='width: 50%;' type='color' id='colorpicker' value='{led_color}'> </input></button> \
                </div> \
            </div> \
        </div> \
    </div> \
</section> \
<script> \  
function changeColor(){ \
    document.getElementById('colorpicker').addEventListener('change', (e) => { \
        let color_value =  e.target.value; \
        console.log(typeof 'color_value');\
        document.getElementById('color_picker').style.backgroundColor = color_value; \
        var xhttp = new XMLHttpRequest();\
        let payload_header = 'color_picker?color=';\
        let payload = payload_header.concat(color_value.substring(1));\
        console.log(payload);\
        xhttp.open(\"GET\",payload, true);\
        xhttp.send();\
    });\
};\
changeColor();\
</script> \
";

const char WEB_PAGE_FOOTER[] PROGMEM = " \
<!-- Footer--> \
    <footer class='py-5 bg-dark'> \
        <div class='container'><p class='m-0 text-center text-white'>Copyright &copy; DMX Controller 2022</p></div> \
    </footer> \  
    <script src='https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js'></script> \  
</body> \
</html> \
";

const char WEB_PAGE_FOOTER_[] PROGMEM = "<br/><br/><a href='/' style='display:inline;'>" WEB_BACK_TO_HOME "</a>"
                "<br/><br/><br/>"
                "</div>\
                <!-- Bootstrap core JS-->\
                </body></html>\r\n";

const char WEB_IOS_REDIRECT[] PROGMEM = "<html><body>Redirecting...\
<script type=\"text/javascript\">\
window.location = \"http://192.168.4.1\";\
</script>\
</body></html>";

#endif //_HTML_CONTENT_H