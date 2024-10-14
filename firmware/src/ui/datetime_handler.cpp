#include "ui/datetime_handler.hpp"

LV_FONT_DECLARE(open_sans_18);
LV_FONT_DECLARE(open_sans_bold_36);
LV_FONT_DECLARE(open_sans_extra_bold_48);

static const char *weekday_char[] = {  //
    "Sunday",   "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"};
static const char *month_char[] = {
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

static bool is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static uint8_t get_weekday(int day, int month, int year) {
  const uint8_t month_key[] = {1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};
  int sum = (year % 100) + ((year % 100) / 4) + day + month_key[month - 1];
  sum -= (month < 3) && is_leap_year(year);
  return ((sum % 7) + 6) % 7;  // Sun: 0, ..., Sat: 6
}

static uint8_t get_calendar_week(struct tm &timeinfo) {
  uint8_t first_weekday = get_weekday(1, 1, timeinfo.tm_year);
  return (timeinfo.tm_yday + first_weekday - 2) / 7 + 1;
}

lv_obj_t *DatetimeHandler::init(lv_obj_t *parent, int size_x, int size_y,
                                const lv_color_t bg_color) {
  BaseHandler::init(parent, size_x, size_y, bg_color);

  lv_obj_t *tile_main, *button_main;
  tile_main = add_event_tile(size_x, size_y, 0, 0, LV_DIR_RIGHT, &button_main);

  auto callback = [](lv_event_t *event) {
    static_cast<DatetimeHandler *>(event->user_data)->event_callback(event);
  };
  lv_obj_add_event_cb(button_main, callback, LV_EVENT_PRESSING, this);

  // Day
  label_day_ = lv_label_create(tile_main);
  lv_obj_set_style_text_color(label_day_, lv_color_black(), 0);
  lv_obj_set_style_text_font(label_day_, &open_sans_extra_bold_48, 0);
  lv_label_set_text(label_day_, "01");
  lv_obj_align(label_day_, LV_ALIGN_TOP_LEFT, 20, 20);

  // Weekday
  label_weekday_ = lv_label_create(tile_main);
  lv_obj_set_style_text_color(label_weekday_, lv_color_black(), 0);
  lv_obj_set_style_text_font(label_weekday_, &open_sans_18, 0);
  lv_label_set_text(label_weekday_, "Wednesday");
  lv_obj_align_to(label_weekday_, label_day_, LV_ALIGN_OUT_TOP_LEFT, 62, 17);

  // Month
  label_month_ = lv_label_create(tile_main);
  lv_obj_set_style_text_color(label_month_, lv_color_black(), 0);
  lv_obj_set_style_text_font(label_month_, &open_sans_18, 0);
  lv_label_set_text(label_month_, "September");
  lv_obj_align_to(label_month_, label_weekday_, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 2);

  // Calendar week
  label_cweek_ = lv_label_create(tile_main);
  lv_obj_set_style_text_color(label_cweek_, lv_color_black(), 0);
  lv_obj_set_style_text_font(label_cweek_, &open_sans_bold_36, 0);
  lv_label_set_text(label_cweek_, "CW01");
  lv_obj_align(label_cweek_, LV_ALIGN_TOP_RIGHT, -20, 20);

  // Time
  label_time_ = lv_label_create(tile_main);
  lv_label_set_recolor(label_time_, true);
  lv_label_set_text(label_time_, "00:00:00");
  lv_obj_set_style_text_color(label_time_, lv_color_black(), 0);
  lv_obj_set_style_text_font(label_time_, &open_sans_extra_bold_48, 0);
  lv_obj_set_style_pad_top(label_time_, 15, 0);
  lv_obj_align(label_time_, LV_ALIGN_CENTER, 0, 10);

  return tileview_;
}

void DatetimeHandler::update() {
  time_t now;
  time(&now);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  timeinfo.tm_year += 1900;

  lv_label_set_text_fmt(label_day_, "%02d", timeinfo.tm_mday);
  lv_label_set_text_fmt(label_weekday_, "%s", weekday_char[timeinfo.tm_wday]);
  lv_label_set_text_fmt(label_month_, "%s", month_char[timeinfo.tm_mon]);
  lv_label_set_text_fmt(label_cweek_, "CW%02d", get_calendar_week(timeinfo));
  lv_label_set_text_fmt(label_time_, "%02d:%02d:%02d", timeinfo.tm_hour,
                        timeinfo.tm_min, timeinfo.tm_sec);
}

void DatetimeHandler::event_callback(lv_event_t *event) {
  if (!status_) return;
  BaseHandler::event_callback(event);
}
