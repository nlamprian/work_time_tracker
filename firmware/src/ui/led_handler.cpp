#include "ui/led_handler.hpp"

#include "config.hpp"

void LedHandler::init(lv_obj_t *parent) {
  led_ = lv_led_create(parent);
  lv_obj_align(led_, LV_ALIGN_TOP_RIGHT, -15, 15);
  lv_led_set_color(led_, lv_palette_main(LV_PALETTE_RED));
  lv_led_set_brightness(led_, LED_BRIGHTNESS);

  auto callback = [](lv_timer_t *event) {
    static_cast<LedHandler *>(event->user_data)->toggle();
  };
  blink_timer_ = lv_timer_create(callback, LED_BLINK_PERIOD, this);
  
  blink_off();
  off();
}
