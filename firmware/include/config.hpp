#pragma once

#include <lvgl.h>

#include <cstdint>

/// MESSAGE IDS /////////////////////////////////////

const uint8_t MSG_SNTP_SYNC = 0x01;
const uint8_t MSG_TOTAL_SW_STATE = 0x02;
const uint8_t MSG_TASK_SW_STATE = 0x03;

/// NETWORK /////////////////////////////////////////

const char WIFI_SSID[] = "";
const char WIFI_PASSWORD[] = "";

const char NTP_SERVER1[] = "pool.ntp.org";
const char NTP_SERVER2[] = "time.nist.gov";
const char GET_TIMEZONE_URL[] = "https://ipapi.co/timezone/";
// Timezone definition including daylight adjustment rules
// List of rules for your timezone can be found in
// https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
const char DEFAULT_TIMEZONE[] = "";

/// GUI /////////////////////////////////////////////

const uint32_t UI_UPDATE_PERIOD = 100;  // [msec]

const uint32_t TOUCHED_UPDATE_PERIOD = 200;  // [msec]
const uint32_t TOUCHED_TIMEOUT = 10000;      // [msec]
const uint8_t SCREEN_BRIGHTNESS_ACTIVE = 220;
const uint8_t SCREEN_BRIGHTNESS_IDLE = 100;

const uint32_t PRESSED_TIMEOUT = 400;  // [msec]
const uint32_t RESET_TIMEOUT = 1000;   // [msec]

const uint32_t LED_BLINK_PERIOD = 600;  // [msec]
const uint8_t LED_BRIGHTNESS = 180;

const lv_color_t TOTAL_SW_COLOR = lv_color_make(0xBB, 0xBB, 0x00);
const lv_color_t TASK_SW_COLOR = lv_color_make(0xBB, 0x00, 0xBB);
