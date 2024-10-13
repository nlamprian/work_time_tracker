#include "ui/stopwatch_handler.hpp"

#include <Arduino.h>

LV_FONT_DECLARE(open_sans_extra_bold_70);

lv_obj_t *StopwatchHandler::init(lv_obj_t *parent, int size_x, int size_y,
                                 const lv_color_t bg_color,
                                 uint8_t state_msg_id) {
  BaseHandler::init(parent, size_x, size_y, bg_color);
  state_msg_id_ = state_msg_id;

  //////////////////////// MAIN ////////////////////////

  lv_obj_t *tile_main, *button_main;
  tile_main = add_event_tile(size_x, size_y, 0, 0, LV_DIR_RIGHT, &button_main);
  auto event_callback = [](lv_event_t *event) {
    static_cast<StopwatchHandler *>(event->user_data)->event_callback(event);
  };
  lv_obj_add_event_cb(button_main, event_callback, LV_EVENT_PRESSED, this);
  lv_obj_add_event_cb(button_main, event_callback, LV_EVENT_PRESSING, this);
  lv_obj_add_event_cb(button_main, event_callback, LV_EVENT_CLICKED, this);

  label_time_ = lv_label_create(tile_main);
  lv_label_set_recolor(label_time_, true);
  lv_label_set_text(label_time_, "00:00:00");
  lv_obj_set_style_text_color(label_time_, color_default_, 0);
  lv_obj_set_style_text_font(label_time_, &open_sans_extra_bold_70, 0);
  lv_obj_center(label_time_);

  led_handler_.init(tile_main);

  //////////////////////// CTRL ////////////////////////
  //
  // lv_obj_t *tile_ctrl = lv_tileview_add_tile(tileview_, 1, 0, LV_DIR_LEFT);
  // lv_obj_t *button_reset = lv_btn_create(tile_ctrl);
  // lv_obj_set_size(button_reset, 100, 50);
  // lv_obj_center(button_reset);
  //
  // label_reset_ = lv_label_create(tile_ctrl);
  // lv_label_set_text(label_reset_, "Reset");
  // lv_obj_center(label_reset_);

  return tileview_;
}

void StopwatchHandler::subscribe(uint8_t msg_id, uint8_t states) {
  sub_states_ = states;
  auto callback = [](void *sub, lv_msg_t *msg) {
    auto stopwatch = static_cast<StopwatchHandler *>(msg->user_data);
    auto payload = *reinterpret_cast<const uint8_t *>(msg->payload);
    if (!(payload & stopwatch->sub_states_)) return;
    if (payload == static_cast<uint8_t>(State::STARTED))
      stopwatch->start();
    else if (payload == static_cast<uint8_t>(State::PAUSED))
      stopwatch->pause();
    else if (payload == static_cast<uint8_t>(State::STOPPED))
      stopwatch->reset();
  };
  lv_msg_subscribe(msg_id, callback, this);
}

// lv_obj_t *Timer::init(lv_obj_t *parent, int size_x, int size_y,
//                       const lv_font_t *font, const lv_color_t bg_color) {
//   tileview_ = lv_obj_create(parent);
//   lv_obj_set_size(tileview_, LV_PCT(size_x), LV_PCT(size_y));
//   lv_obj_set_scroll_snap_x(tileview_, LV_SCROLL_SNAP_CENTER);
//   lv_obj_set_flex_flow(tileview_, LV_FLEX_FLOW_ROW);
//   lv_obj_add_flag(tileview_, LV_OBJ_FLAG_SCROLL_ONE);
// //   lv_obj_update_snap(tileview_, LV_ANIM_ON);
//   lv_obj_set_style_bg_color(tileview_, bg_color, 0);
//   lv_obj_set_scrollbar_mode(tileview_, LV_SCROLLBAR_MODE_OFF);
//   //   lv_obj_add_event_cb(tileview_, event_callback,
//   //                       LV_EVENT_VALUE_CHANGED, this);
//
//   label_time_ = lv_label_create(tileview_);
//   lv_obj_set_size(label_time_, LV_PCT(static_cast<int>(size_x * 1.2)),
//   LV_PCT(size_y)); lv_label_set_recolor(label_time_, true);
//   lv_label_set_text(label_time_, "12:34:56");
//   lv_obj_set_style_text_color(label_time_, lv_color_black(), 0);
//   lv_obj_set_style_text_font(label_time_, font, 0);
//   // lv_obj_align(label_time_, LV_ALIGN_CENTER, 0, 0);
//   lv_obj_center(label_time_);
//
// //   label_reset_ = lv_label_create(tileview_);
// // //   lv_obj_set_size(label_reset_, LV_PCT(size_x), LV_PCT(size_y));
// //   lv_label_set_text(label_reset_, "Duration");
// //   lv_obj_align(label_reset_, LV_ALIGN_CENTER, 0, 0);
//
//   return tileview_;
// }

void StopwatchHandler::update() {
  if (on_reset_ && lv_tick_elaps(reset_start_time_) > RESET_TIMEOUT)
    set_duration_view();

  if (state_ != State::STARTED || on_reset_) return;

  uint32_t current_time = millis();
  duration_ += current_time - last_update_time_;
  last_update_time_ = current_time;

  update_duration(duration_);
}

void StopwatchHandler::event_callback(lv_event_t *event) {
  if (!status_) return;
  BaseHandler::event_callback(event);

  lv_event_code_t code = lv_event_get_code(event);
  if (code == LV_EVENT_PRESSED) {
    Serial.println("Pressed");

    pressed_start_time_ = lv_tick_get();
    on_long_pressed_ = false;

  } else if (code == LV_EVENT_PRESSING) {
    Serial.println("Pressing");

    if (on_long_pressed_ or
        lv_tick_elaps(pressed_start_time_) < PRESSED_TIMEOUT)
      return;

    Serial.println("Long pressed");

    set_reset_view();

  } else if (code == LV_EVENT_CLICKED) {
    Serial.println("Clicked");

    // The user stopped pressing the button
    if (on_long_pressed_) {
      on_long_pressed_ = false;
      return;
    }

    // The user requested to reset the timer
    if (on_reset_) {
      reset();
      return;
    }

    if (state_ == State::STARTED) {
      pause();
    } else {
      start();
    }
  }
}

// void Timer::event_tile_callback(lv_event_t *event) {
//   lv_event_code_t code = lv_event_get_code(event);
//   if (code == LV_EVENT_VALUE_CHANGED) {
//     static int count = 0;
//     int current_tile_id =
//     lv_obj_get_index(lv_tileview_get_tile_act(tileview_)); int target_tile_id
//     = lv_obj_get_index(event->target); if (current_tile_id != 0 ||
//     target_tile_id != 1) return; lv_label_set_text_fmt(label_reset_, "%d",
//     count++); Serial.printf("current: %d, target: %d\n", current_tile_id,
//     target_tile_id);
//   }
// }

void StopwatchHandler::set_duration_view() {
  on_reset_ = false;
  update_duration(duration_);
  led_handler_.show();
  if (state_ == State::PAUSED) {
    lv_obj_set_style_text_color(label_time_, color_paused_, 0);
    led_handler_.blink_on();
  }
}

void StopwatchHandler::set_reset_view() {
  on_long_pressed_ = true;
  on_reset_ = true;
  reset_start_time_ = lv_tick_get();
  if (state_ == State::PAUSED) led_handler_.blink_off();
  led_handler_.hide();
  lv_obj_set_style_text_color(label_time_, color_default_, 0);
  lv_label_set_text_fmt(label_time_, "Reset");
}

void StopwatchHandler::update_duration(uint32_t duration_ms) {
  uint32_t total_seconds = duration_ms / 1000;
  uint32_t hours = total_seconds / 3600;
  uint32_t minutes = (total_seconds % 3600) / 60;
  uint32_t seconds = total_seconds % 60;
  lv_label_set_text_fmt(label_time_, "%02d:%02d:%02d", hours, minutes, seconds);
}

void StopwatchHandler::send_state_msg(State state) {
  state_msg_payload_ = static_cast<uint8_t>(state);
  lv_msg_send(state_msg_id_, &state_msg_payload_);
}

void StopwatchHandler::start() {
  last_update_time_ = millis();
  state_ = State::STARTED;
  lv_obj_set_style_text_color(label_time_, color_default_, 0);
  led_handler_.blink_off();
  led_handler_.on();
  send_state_msg(State::STARTED);
  Serial.println("Started");
}

void StopwatchHandler::pause() {
  if (state_ != State::STARTED) return;
  state_ = State::PAUSED;
  lv_obj_set_style_text_color(label_time_, color_paused_, 0);
  led_handler_.blink_on();
  send_state_msg(State::PAUSED);
  Serial.println("Paused");
}

void StopwatchHandler::reset() {
  led_handler_.blink_off();
  led_handler_.show();
  led_handler_.off();
  state_ = State::STOPPED;
  on_reset_ = false;
  lv_obj_set_style_text_color(label_time_, color_default_, 0);
  update_duration(duration_ = 0);
  send_state_msg(State::STOPPED);
  Serial.println("Reset");
}
