#pragma once

#include <AceButton.h>
#include <LilyGo_AMOLED.h>

#include "wifi_handler.hpp"

using namespace ace_button;

class DeviceButtonEventHandler;

class DeviceHandler {
  friend class DeviceButtonEventHandler;

 public:
  using ScreenStatusCallbackType = std::function<void(bool)>;

  ~DeviceHandler();
  inline LilyGo_Class &get_screen() { return screen_; }
  void init(WiFiHandler *wifi);
  void toggle_screen();
  void set_brightness(bool active);
  inline bool is_screen_on() { return is_screen_on_; }
  inline bool in_sleep_mode() { return in_sleep_mode_; }
  void set_screen_status_callback(const ScreenStatusCallbackType &callback);

 protected:
  void button_check_task();

  LilyGo_Class screen_;

  AceButton *buttons_ = nullptr;
  DeviceButtonEventHandler *button_event_handlers_ = nullptr;

  bool is_screen_on_ = true;
  volatile bool in_sleep_mode_ = false;

  ScreenStatusCallbackType screen_status_callback_ = nullptr;
};

class DeviceButtonEventHandler : public IEventHandler {
 public:
  void init(DeviceHandler *device, WiFiHandler *wifi);
  void handleEvent(AceButton *button, uint8_t eventType,
                   uint8_t buttonState) override;

 protected:
  DeviceHandler *device_;
  WiFiHandler *wifi_;
};
