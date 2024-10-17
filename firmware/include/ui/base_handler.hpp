#pragma once

#include <lvgl.h>

#include "config.hpp"

class BaseHandler {
 public:
  virtual lv_obj_t *init(lv_obj_t *parent, int size_x, int size_y,
                         lv_color_t bg_color);
  inline bool touched() const { return touched_; }
  virtual void update() = 0;

 protected:
  virtual void event_callback(lv_event_t *event);
  
  /**
   * @brief Adds an eventful tile.
   * @details Adds a tile with a transparent button that covers the whole tile
   * and can be used to detect touch events.
   */
  lv_obj_t *add_event_tile(int size_x, int size_y, uint8_t col_id,
                           uint8_t row_id, lv_dir_t dir, lv_obj_t **button);

  lv_obj_t *tileview_;

 private:
  void update_touched_callback();

  lv_timer_t *touched_timer_;
  volatile uint32_t touched_time_;
  volatile bool touched_ = true;
};
