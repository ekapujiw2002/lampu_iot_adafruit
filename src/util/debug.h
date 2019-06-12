#ifndef _DEBUG_UTIL_H_
#define _DEBUG_UTIL_H_

// main lib
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#define APP_DEBUG_PORT Serial
#define APP_DEBUG_SPEED 57600

#ifdef APP_DEBUG_PORT
#ifdef ESP8266
#define APP_DEBUG_PRINT_ARGS(...) APP_DEBUG_PORT.printf(__VA_ARGS__)
#else
#define APP_DEBUG_PRINT_ARGS(...) APP_DEBUG_PORT.print(__VA_ARGS__)
#endif
#else
#define APP_DEBUG_PRINT_ARGS(...)
#endif

// proto func
void APP_DEBUG_INIT(const long speed);
void APP_DEBUG_PRINT_HEADER();
void APP_DEBUG_PRINT(String alog);
void APP_DEBUG_PRINT_P(const __FlashStringHelper *alog);

/**
 * init debug port setting
 * @method APP_DEBUG_INIT
 * @param  speed          [description]
 */
void APP_DEBUG_INIT(const long speed) {
#ifdef APP_DEBUG_PORT
  // APP_DEBUG_PORT.begin(speed);
  APP_DEBUG_PORT.begin(speed, SERIAL_8N1, SERIAL_TX_ONLY, 1);
#ifdef ESP8266
  APP_DEBUG_PORT.setDebugOutput(true);
#endif
#endif
}

/**
 * print debug header
 * @method APP_DEBUG_PRINT_HEADER
 */
void APP_DEBUG_PRINT_HEADER() {
#ifdef APP_DEBUG_PORT
// char dtx[64] = {0};
// snprintf_P(dtx, sizeof(dtx), (const char *)F("%-10u : "), millis());
// APP_DEBUG_PORT.print(dtx);
#ifdef ESP8266
  APP_DEBUG_PRINT_ARGS("%-10lu : ", millis());
#else
  APP_DEBUG_PORT.print(millis());
  APP_DEBUG_PORT.print(" : ");
#endif
#endif
}

/**
 * debug printing util
 * @method APP_DEBUG_PRINT
 * @param  alog            [description]
 */

void APP_DEBUG_PRINT(String alog) {
#ifdef APP_DEBUG_PORT
  APP_DEBUG_PRINT_HEADER();
  APP_DEBUG_PORT.println(alog);
#endif
}

/**
 * flash serial debug print version
 * @method APP_DEBUG_PRINT_P
 * @param  alog              flash string
 */
void APP_DEBUG_PRINT_P(const __FlashStringHelper *alog) {
#ifdef APP_DEBUG_PORT
  APP_DEBUG_PRINT_HEADER();
  APP_DEBUG_PORT.println(alog);
#endif
}

#ifdef __cplusplus
}
#endif

#endif
