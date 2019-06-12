#ifndef __RTC__DISPLAY_H__
#define __RTC__DISPLAY_H__

#include <Arduino.h>
#ifdef ESP8266
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

#include "sipo595.h"

#ifdef __cplusplus
extern "C" {
#endif

// max segment length
#define RTC_DISPLAY_MAX_LEN 4

// special character
#define RTC_DISPLAY_CHAR_SPACE 0xff
#define RTC_DISPLAY_CHAR_STRIP 0xbf
#define RTC_DISPLAY_CHAR_DONT_DISPLAY 0x01
#define RTC_DISPLAY_CHAR_M1 0xcc
#define RTC_DISPLAY_CHAR_M2 0xd8

// number font
static const uint8_t number[] PROGMEM = {
    // 0  ,1    ,2    ,3    ,4    ,5    ,6    ,7    ,8    ,9
    0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};

// character font
static const uint8_t character[] PROGMEM = {
    // A  ,B    ,C    ,D    ,E    ,F    ,G    ,H    ,I
    0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xc2, 0x89, 0xcf,
    // J  ,K    ,L    ,M    ,N    ,O    ,P    ,Q    ,R
    0xe1, 0x8a, 0xc7, 0xea, 0xc8, 0xc0, 0x8c, 0x98, 0xcc,
    // S  ,T    ,U    ,V    ,W    ,X    ,Y    ,Z
    0x92, 0x87, 0xc1, 0xd1, 0xd5, 0x89, 0x91, 0xb4};

// buffer for text
#define RTC_DISPLAY_BUFFER_MAX_LENGTH 80

// global var
char rtc_display_buffer[RTC_DISPLAY_BUFFER_MAX_LENGTH];
const char *rtc_locale_day_name[] = {" ", "MINGGU", "SENIN",
                                     "SELASA", "RABU", "KAMIS",
                                     "JUMAT", "SABTU"};
// const char *rtc_iqamah_text = "IQO<>AH ";

// proto function
#define rtc_display_initialize(pin_din, pin_clock, pin_latch, latch_delay,     \
                               clock_delay)                                    \
  ({                                                                           \
    sipo595_initialize(pin_din, pin_clock, pin_latch, latch_delay,             \
                       clock_delay);                                           \
  })
void rtc_display_clear_text();
void rtc_display_show_text(char *text);

/**
 * clear display text
 * @method rtc_display_clear_text
 */
void rtc_display_clear_text() {
  for (size_t i = 0; i < RTC_DISPLAY_MAX_LEN; i++) {
    sipo595_write(0xff, (i == (RTC_DISPLAY_MAX_LEN - 1)));
  }
}

/**
 * show text to display
 * @method rtc_display_show_text
 * @param  text                  [description]
 */
void rtc_display_show_text(char *text) {
  uint8_t id, max_len_text, data_sipo, is_dot_on;

  max_len_text = strlen(text);
  // if (strlen(text) > RTC_DISPLAY_MAX_LEN) {
  //   max_len_text = RTC_DISPLAY_MAX_LEN;
  // }

  is_dot_on = 0;
  id = RTC_DISPLAY_MAX_LEN;
  while (max_len_text && id) {
    // spasi
    if (text[max_len_text - 1] == ' ') {
      data_sipo = RTC_DISPLAY_CHAR_SPACE & (is_dot_on ? (~0x80) : 0xff);
      is_dot_on = 0;
      // strip
    } else if (text[max_len_text - 1] == '-') {
      data_sipo = RTC_DISPLAY_CHAR_STRIP;

      // 0-9
    } else if ((text[max_len_text - 1] >= '0') &&
               (text[max_len_text - 1] <= '9')) {
      data_sipo = pgm_read_byte(number + text[max_len_text - 1] - '0') &
                  (is_dot_on ? (~0x80) : 0xff);
      is_dot_on = 0;

      // a-z dan A-Z
    } else if (((text[max_len_text - 1] >= 'a') &&
                (text[max_len_text - 1] <= 'z')) ||
               ((text[max_len_text - 1] >= 'A') &&
                (text[max_len_text - 1] <= 'Z'))) {
      data_sipo = pgm_read_byte(character + text[max_len_text - 1] -
                                ((text[max_len_text - 1] >= 'a') ? 'a' : 'A')) &
                  (is_dot_on ? (~0x80) : 0xff);
      is_dot_on = 0;
    }

    //. or :
    else if ((text[max_len_text - 1] == ':') ||
             (text[max_len_text - 1] == '.')) {
      if (max_len_text > 0) {
        is_dot_on = 1;
      } else {
        is_dot_on = 0;
        data_sipo = (RTC_DISPLAY_CHAR_SPACE & (~0x80));
      }

      // tanda < dan > untuk ganti M
    } else if (text[max_len_text - 1] == '<') {
      data_sipo = RTC_DISPLAY_CHAR_M1;
    } else if (text[max_len_text - 1] == '>') {
      data_sipo = RTC_DISPLAY_CHAR_M2;
    }

    // others
    else {
      data_sipo = RTC_DISPLAY_CHAR_DONT_DISPLAY;
      is_dot_on = 0;
    }

    // decrement id text
    max_len_text--;

    if ((!is_dot_on) && (data_sipo != RTC_DISPLAY_CHAR_DONT_DISPLAY)) {
      // decrement counter max display len
      id--;

      sipo595_write(data_sipo, (id == 0));
    }
  }

  // make sur eit is latched
  sipo595_latch_pulse();
}

#ifdef __cplusplus
}
#endif
#endif
