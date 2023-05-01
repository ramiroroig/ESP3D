#include "../../../include/esp3d_config.h"
#if defined(HTTP_FEATURE) && defined(SD_DEVICE)
#include "../http_server.h"
#if defined(ARDUINO_ARCH_ESP32)
#include <WebServer.h>
#endif  // ARDUINO_ARCH_ESP32
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WebServer.h>
#endif  // ARDUINO_ARCH_ESP8266
#include "../../authentication/authentication_service.h"
#include "../../filesystem/esp_sd.h"
#include "../../../core/commands.h"


void HTTP_Server::handleOctoPrinter() {
    _webserver->send(200, "application/json", "{\"sd\":{\"ready\": true},\"state\":{\"flags\":{\"operational\": false,\"sdReady\": true,\"ready\": true}}}");
};

void HTTP_Server::handleOCtoVersion() {
    _webserver->send(200, "application/json", "{\"api\":\"0.1\",\"server\":\"2.0.6\",\"text\":\"OctoPrint ESP32 2.0.6\"}");
};

void HTTP_Server::handleOctoFile() {
    level_authenticate_type auth_level = AuthenticationService::authenticated_level();
    if (auth_level == LEVEL_GUEST) {
        _upload_status = UPLOAD_STATUS_NONE;
        _webserver->send(401, "text/plain", "Wrong authentication!");
        return;
    }

    if ((_upload_status == UPLOAD_STATUS_FAILED) ||
        (_upload_status == UPLOAD_STATUS_CANCELLED)) {
        _upload_status = UPLOAD_STATUS_NONE;
        _webserver->send(500, "text/plain", "Upload failed!");
        return;
    }

    _webserver->send(200, "application/json", "{\"done\":true}");

    if(_webserver->hasArg("print")) {
        if(_webserver->arg("print") == "true") {
            ESP3DOutput output;
            String cmd = "[ESP700]/SD" + last_upload_file;
            esp3d_commands.process((uint8_t*)cmd.c_str(), cmd.length(), &output, auth_level);
        }
    }
};

#endif