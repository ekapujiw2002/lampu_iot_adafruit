#ifndef __SIPO595_UTIL_H__
#define __SIPO595_UTIL_H__

// main library
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t sipo595_pin_din, sipo595_pin_clock, sipo595_pin_latch;
uint16_t sipo595_latch_delay = 1, sipo595_clock_delay = 0;

// proto void
void sipo595_initialize(uint8_t pin_din, uint8_t pin_clock, uint8_t pin_latch,
                        uint16_t latch_delay, uint16_t clock_delay);
void sipo595_clock_pulse();
void sipo595_latch_pulse();
void sipo595_write(uint8_t data, uint8_t is_latched);

/**
 * init sipo pin
 * @method spio595_initialize
 * @param  pin_din            data input pin
 * @param  pin_clock          clock pin
 * @param  pin_latch          latch pin
 */
void sipo595_initialize(uint8_t pin_din, uint8_t pin_clock, uint8_t pin_latch,
                        uint16_t latch_delay, uint16_t clock_delay) {
  sipo595_pin_din = pin_din;
  sipo595_pin_clock = pin_clock;
  sipo595_pin_latch = pin_latch;
  sipo595_latch_delay = latch_delay;
  sipo595_clock_delay = clock_delay;

  digitalWrite(sipo595_pin_din, LOW);
  digitalWrite(sipo595_pin_clock, LOW);
  digitalWrite(sipo595_pin_latch, LOW);

  pinMode(sipo595_pin_din, OUTPUT);
  pinMode(sipo595_pin_clock, OUTPUT);
  pinMode(sipo595_pin_latch, OUTPUT);
}

/**
 * pulse clock with delay if added
 * @method sipo595_clock_pulse
 * @param  delay_clock         delay in microseconds
 */
void sipo595_clock_pulse() {
  digitalWrite(sipo595_pin_clock, HIGH);
  if (sipo595_clock_delay != 0) {
    delayMicroseconds(sipo595_clock_delay);
  }
  digitalWrite(sipo595_pin_clock, LOW);
  if (sipo595_clock_delay != 0) {
    delayMicroseconds(sipo595_clock_delay);
  }
}

/**
 * latch the 595
 * @method sipo595_latch_pulse
 */
void sipo595_latch_pulse() {
  digitalWrite(sipo595_pin_latch, HIGH);
  if (sipo595_latch_delay != 0) {
    delayMicroseconds(sipo595_latch_delay);
  }
  digitalWrite(sipo595_pin_latch, LOW);
  if (sipo595_latch_delay != 0) {
    delayMicroseconds(sipo595_latch_delay);
  }
}

/**
 * send 1 byte to 595
 * @method sipo595_write
 * @param  data          the data
 * @param  is_latched    latch or not , 0=not latch, 1=latched
 */
void sipo595_write(uint8_t data, uint8_t is_latched) {
  // Send each 8 bits serially
  uint8_t tmpd = 0;

  // Order is MSB first
  uint8_t i;

  tmpd = data;
  for (i = 0; i < 8; i++) {
    // Output the data on DS line according to the
    // Value of MSB

    digitalWrite(sipo595_pin_din, (tmpd & 0b10000000) ? HIGH : LOW);

    sipo595_clock_pulse(); // Pulse the Clock line
    tmpd = tmpd << 1;      // Now bring next bit at MSB position
  }

  // Now all 8 bits have been transferred to shift register
  // Move them to output latch at one
  if (is_latched == 1)
    sipo595_latch_pulse();
}

#ifdef __cplusplus
}
#endif
#endif
