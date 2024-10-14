#pragma once

#include <lvgl.h>
#include <time.h>

#include "config.hpp"
#include "ui/base_handler.hpp"

class DatetimeHandler : public BaseHandler {
 public:
  lv_obj_t *init(lv_obj_t *parent, int size_x, int size_y, lv_color_t bg_color);
  void update() override;

 protected:
  void event_callback(lv_event_t *event) override;

  lv_obj_t *label_day_;
  lv_obj_t *label_weekday_;
  lv_obj_t *label_month_;
  lv_obj_t *label_cweek_;
  lv_obj_t *label_time_;
};
