#include "device_handler.hpp"

#include <Arduino.h>
#include <task.h>

#include "config.hpp"

DeviceHandler::~DeviceHandler() {
  if (buttons_) delete[] buttons_;
  if (button_event_handlers_) delete[] button_event_handlers_;
}

void DeviceHandler::init(WiFiHandler *wifi) {
  bool success = screen_.beginAMOLED_241();
  if (!success) {
    Serial.println("Failed to detect the board model");
    while (true) delay(1000);
  }
  screen_.setRotation(1);

  Serial.println("\n=======================================");
  Serial.printf("  Board Name: LilyGo AMOLED %s\n", screen_.getName());
  Serial.println("=======================================\n");

  const BoardsConfigure_t *config = screen_.getBoardsConfigure();
  if (config->buttonNum) {
    Serial.printf("Configuring %d device buttons\n", config->buttonNum);
    buttons_ = new AceButton[config->buttonNum];
    button_event_handlers_ = new DeviceButtonEventHandler[config->buttonNum];

    ButtonConfig *button_config;
    for (int i = 0; i < config->buttonNum; ++i) {
      pinMode(config->pButtons[i], INPUT_PULLUP);
      buttons_[i].init(config->pButtons[i], HIGH, i);
      button_event_handlers_[i].init(this, wifi);
      button_config = buttons_[i].getButtonConfig();
      button_config->setFeature(ButtonConfig::kFeatureClick);
      button_config->setFeature(ButtonConfig::kFeatureLongPress);
      button_config->setIEventHandler(&button_event_handlers_[i]);
    }

    auto callback = [](void *ptr) {
      static_cast<DeviceHandler *>(ptr)->button_check_task();
    };
    xTaskCreate(callback, "button_check", 5 * 1024, this, 12, nullptr);
  }
}

void DeviceHandler::toggle_screen() {
  is_screen_on_ = !is_screen_on_;
  Serial.printf("Setting screen %s\n", is_screen_on_ ? "on" : "off");
  screen_.setBrightness(is_screen_on_ ? SCREEN_BRIGHTNESS_ACTIVE : 0);
  if (screen_status_callback_) screen_status_callback_(is_screen_on_);
}

void DeviceHandler::set_brightness(bool active) {
  Serial.printf("Setting brightness to %s\n", active ? "ACTIVE" : "IDLE");
  screen_.setBrightness(active ? SCREEN_BRIGHTNESS_ACTIVE
                               : SCREEN_BRIGHTNESS_IDLE);
}

void DeviceHandler::set_screen_status_callback(
    const DeviceHandler::ScreenStatusCallbackType &callback) {
  screen_status_callback_ = callback;
}

void DeviceHandler::button_check_task() {
  const BoardsConfigure_t *config = screen_.getBoardsConfigure();
  while (true) {
    for (int i = 0; i < config->buttonNum; ++i) buttons_[i].check();
    delay(5);
  }
  vTaskDelete(nullptr);
}

void DeviceButtonEventHandler::init(DeviceHandler *device, WiFiHandler *wifi) {
  device_ = device;
  wifi_ = wifi;
}

void DeviceButtonEventHandler::handleEvent(AceButton *button,
                                           uint8_t event_type,
                                           uint8_t button_state) {
  Serial.print("Button event (type: ");
  Serial.print(AceButton::eventName(event_type));
  Serial.printf(", state: %d)\n", button_state);

  switch (event_type) {
    case AceButton::kEventClicked:
      device_->toggle_screen();
      break;
    case AceButton::kEventLongPressed:
      device_->in_sleep_mode_ = true;

      Serial.println("Going to sleep...");
      wifi_->disconnect();
      device_->screen_.sleep(true);

      // Set BOOT button as wakeup source
      esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, LOW);

      Wire.end();

      delay(1000);
      esp_deep_sleep_start();
      break;
    default:
      break;
  }
}
