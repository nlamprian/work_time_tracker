#include "ui/ui_manager.hpp"

#include <LV_Helper.h>

#include "config.hpp"

UiManager::UiManager(DeviceHandler &device) : device_(device) {}

void UiManager::init() {
  beginLvglHelper(device_.get_screen(), false);
  lv_obj_t *parent = lv_scr_act();

  lv_color_t color = lv_color_white();
  lv_obj_t *date_screen = datetime_.init(parent, 100, 40, color);

  lv_obj_t *total_time =
      sw_total_.init(parent, 100, 30, TOTAL_SW_COLOR, MSG_TOTAL_SW_STATE);
  lv_obj_align_to(total_time, date_screen, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  lv_obj_t *task_time =
      sw_task_.init(parent, 100, 30, TASK_SW_COLOR, MSG_TASK_SW_STATE);
  lv_obj_align_to(task_time, total_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  using State = StopwatchHandler::State;
  auto get_value = [](State state) { return static_cast<uint8_t>(state); };
  sw_total_.subscribe(MSG_TASK_SW_STATE, get_value(State::STARTED));
  sw_task_.subscribe(MSG_TOTAL_SW_STATE,
                     get_value(State::PAUSED) | get_value(State::STOPPED));

  auto callback = [](void *sub, lv_msg_t *msg) {
    static_cast<UiManager *>(msg->user_data)->sntp_sync_callback();
    lv_msg_unsubscribe(sub);
  };
  lv_msg_subscribe(MSG_SNTP_SYNC, callback, this);

  Serial.println("UI is ready");
}

void UiManager::sntp_sync_callback() {
  Serial.println("Datetime is synced");
  auto callback = [](lv_timer_t *event) {
    static_cast<UiManager *>(event->user_data)->update_callback();
  };
  time_timer_ = lv_timer_create(callback, UI_UPDATE_PERIOD, this);
}

void UiManager::update_callback() {
  static bool force = false;
  if (!device_.is_screen_on()) {
    force = true;
    return;
  }

  datetime_.update();
  sw_total_.update();
  sw_task_.update();

  bool touched =
      datetime_.touched() || sw_total_.touched() || sw_task_.touched();
  if (touched != last_touched_ || force) {
    device_.set_brightness(touched);
    force = false;
  }
  last_touched_ = touched;
}
