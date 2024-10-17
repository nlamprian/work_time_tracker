#pragma once

#include <lvgl.h>

class LedHandler {
 public:
  void init(lv_obj_t *parent);
  inline void on() { lv_led_on(led_); }
  inline void off() { lv_led_off(led_); }
  inline void toggle() { lv_led_toggle(led_); }
  inline void show() { lv_obj_clear_flag(led_, LV_OBJ_FLAG_HIDDEN); }
  inline void hide() { lv_obj_add_flag(led_, LV_OBJ_FLAG_HIDDEN); }
  inline void blink_on() { lv_timer_resume(blink_timer_); }
  inline void blink_off() { lv_timer_pause(blink_timer_); }

 protected:
  lv_obj_t *led_;
  lv_timer_t *blink_timer_;
};
