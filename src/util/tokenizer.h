#ifndef _TOKEN_UTIL_H_
#define _TOKEN_UTIL_H_

// main library
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

// proto function void
String get_value(String data, char separator, int index);

/**
 * get value from string delimited by char
 * @method getValue
 * @param  data      the string input_string
 * @param  separator char separator data
 * @param  index     index data from 0
 * @return           string
 */
String get_value(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

#ifdef __cplusplus
}
#endif

#endif
