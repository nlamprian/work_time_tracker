#pragma once

#include <WiFi.h>

#include <functional>

class WiFiHandler {
 public:
  static WiFiHandler *get();
  void init();
  void disconnect();
  void check();

 private:
  /**
   * @brief Configures datetime once network connection is established.
   */
  void wifi_event_callback(WiFiEvent_t event, WiFiEventInfo_t info);

  wifi_event_id_t wifi_event_id_;
  uint32_t next_check_time_ = 2000;
};
