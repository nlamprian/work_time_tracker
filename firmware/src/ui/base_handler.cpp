#include "ui/base_handler.hpp"

lv_obj_t *BaseHandler::init(lv_obj_t *parent, int size_x, int size_y,
                            const lv_color_t bg_color) {
  tileview_ = lv_tileview_create(parent);
  lv_obj_set_size(tileview_, LV_PCT(size_x), LV_PCT(size_y));
  lv_obj_set_style_bg_color(tileview_, bg_color, 0);
  lv_obj_set_scrollbar_mode(tileview_, LV_SCROLLBAR_MODE_OFF);
  
  // auto callback = [](lv_event_t *event) {
  //   static_cast<BaseHandler *>(event->user_data)->event_callback(event);
  // };
  // lv_obj_add_event_cb(tileview_, callback, LV_EVENT_VALUE_CHANGED, this);

  auto callback = [](lv_timer_t *event) {
    static_cast<BaseHandler *>(event->user_data)->update_touched_callback();
  };
  touched_timer_ = lv_timer_create(callback, TOUCHED_UPDATE_PERIOD, this);

  return tileview_;
}

void BaseHandler::event_callback(lv_event_t *event) {
  touched_time_ = lv_tick_get();
  touched_ = true;
}

lv_obj_t *BaseHandler::add_event_tile(int size_x, int size_y, uint8_t col_id,
                                      uint8_t row_id, lv_dir_t dir,
                                      lv_obj_t **button) {
  lv_obj_t *tile = lv_tileview_add_tile(tileview_, col_id, row_id, dir);
  *button = lv_btn_create(tile);
  lv_obj_set_size(*button, LV_PCT(size_x), LV_PCT(size_y));
  lv_obj_align(*button, LV_ALIGN_CENTER, 0, 0);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_radius(&style, 0);
  lv_style_set_bg_opa(&style, LV_OPA_0);
  lv_style_set_shadow_width(&style, 0);
  lv_obj_add_style(*button, &style, 0);

  return tile;
}

void BaseHandler::update_touched_callback() {
  if (touched_ && lv_tick_elaps(touched_time_) > TOUCHED_TIMEOUT)
    touched_ = false;
}
