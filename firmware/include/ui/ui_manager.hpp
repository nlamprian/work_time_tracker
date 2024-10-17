#pragma once

#include <lvgl.h>

#include "device_handler.hpp"
#include "ui/datetime_handler.hpp"
#include "ui/stopwatch_handler.hpp"

class UiManager {
 public:
  UiManager(DeviceHandler &device);
  void init();

 private:
  /**
   * @brief Starts the UI update timer.
   */
  void sntp_sync_callback();
  void update_callback();

  DeviceHandler &device_;
  bool last_touched_ = true;

  DatetimeHandler datetime_;
  StopwatchHandler sw_total_;
  StopwatchHandler sw_task_;

  lv_timer_t *time_timer_;
};
