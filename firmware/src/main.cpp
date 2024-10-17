#include <Arduino.h>
#include <functional>

#include "config.hpp"
#include "device_handler.hpp"
#include "ui/ui_manager.hpp"
#include "wifi_handler.hpp"

WiFiHandler *wifi = WiFiHandler::get();
DeviceHandler device;
UiManager ui(device);

void setup() {
  Serial.begin(115200);
  device.init(wifi);
  ui.init();
  wifi->init();
  enableLoopWDT();
}

void loop() {
  if (device.in_sleep_mode()) return;
  wifi->check();
  lv_task_handler();
  delay(1);
}
