#include "wifi_handler.hpp"

#include <Arduino.h>
#include <esp_sntp.h>
#include <lvgl.h>

#include "config.hpp"

WiFiHandler *WiFiHandler::get() {
  static WiFiHandler instance;
  return &instance;
}

void WiFiHandler::init() {
  auto callback = std::bind(&WiFiHandler::wifi_event_callback, this,
                            std::placeholders::_1, std::placeholders::_2);
  wifi_event_id_ = WiFi.onEvent(callback);
  WiFi.mode(WIFI_STA);

  Serial.printf("WiFi SSID: %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void WiFiHandler::disconnect() {
  WiFi.disconnect();
  WiFi.removeEvent(wifi_event_id_);
  WiFi.mode(WIFI_OFF);
}

void WiFiHandler::check() {
  if (next_check_time_ > millis()) return;
  next_check_time_ = millis() + 5000;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
}

void WiFiHandler::wifi_event_callback(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_READY:
      Serial.println("WiFi interface is ready");
      break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
      Serial.println("Completed scan for access points");
      break;
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("WiFi client started");
      break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
      Serial.println("WiFi client stopped");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("Connected to access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Disconnected from access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
      Serial.println("Authentication mode of access point has changed");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP: {
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());

      auto callback = [](struct timeval *time) {
        lv_msg_send(MSG_SNTP_SYNC, nullptr);
      };
      sntp_set_time_sync_notification_cb(callback);
      configTzTime(DEFAULT_TIMEZONE, NTP_SERVER1, NTP_SERVER2);

    } break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      Serial.println("Lost IP address");
      break;
    default:
      break;
  }
}
