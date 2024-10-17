#pragma once

#include <lvgl.h>
#include <time.h>

#include "config.hpp"
#include "ui/base_handler.hpp"
#include "ui/led_handler.hpp"

class StopwatchHandler : public BaseHandler {
 public:
  enum class State : uint8_t { STARTED = 0x01, PAUSED = 0x02, STOPPED = 0x04 };

  virtual lv_obj_t *init(lv_obj_t *parent, int size_x, int size_y,
                         lv_color_t bg_color, uint8_t state_msg_id);
  void subscribe(uint8_t msg_id, uint8_t states);
  void update() override;

 protected:
  void event_callback(lv_event_t *event) override;
  void set_duration_view();
  void set_reset_view();
  void update_duration(uint32_t duration);
  void send_state_msg(State state);
  void start();
  void pause();
  void reset();

  uint8_t state_msg_id_;
  uint8_t state_msg_payload_;
  uint8_t sub_states_;

  lv_obj_t *led_;
  lv_obj_t *label_time_;
  lv_obj_t *label_reset_;

  State state_ = State::STOPPED;
  uint32_t last_update_time_ = 0;  // [msec]
  uint32_t duration_ = 0;          // [msec]

  bool on_long_pressed_ = false;
  uint32_t pressed_start_time_;

  bool on_reset_ = false;
  uint32_t reset_start_time_;

  LedHandler led_handler_;
};
