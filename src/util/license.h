#ifndef __LICENSE_GEN_H__
#define __LICENSE_GEN_H__

#include <Arduino.h>
#include <sha256.h>

#ifdef __cplusplus
extern "C" {
#endif

Sha256 lic_engine;

// key, any character with any length will do
uint8_t lic_key[] = {"pBq4kiLNmXh5mou09QmF"};
// uint8_t lic_key[] = {
//     "\xd7\xda\x9f\x13\xc9\xd3\x48\x66\x93\xa8\xdc\x56\xf5\x2a\x23\x16"};
// uint8_t lic_key[] = {"1234567890123456"};

// buffer for ascii and raw data
uint8_t lic_buffer[BLOCK_LENGTH + 1] = {0};
uint8_t *lic_ascii_buffer = &lic_buffer[0];
uint8_t lic_stsx = 0;

// proto void
#define lic_validate_key(input_sn) ({ memcmp(lic_ascii_buffer, input_sn, 64); })
uint8_t *lic_gen_sn(char *input_data);
void lic_raw_to_ascii();
void lic_obsfuscate_raw_buffer();

/**
 * just swapping byte for more confusion
 * @method lic_obsfuscate_raw_buffer
 */
void lic_obsfuscate_raw_buffer() {
  uint8_t tmpx;
  for (size_t i = 0; i < (HASH_LENGTH >> 1); i++) {
    tmpx = lic_buffer[32 + i];
    lic_buffer[32 + i] = lic_buffer[63 - i];
    lic_buffer[63 - i] = tmpx;
  }
}

/**
 * convert raw buffer to ascii buffer
 * end with null
 * @method lic_raw_to_ascii
 */
void lic_raw_to_ascii() {
  for (size_t i = 0; i < HASH_LENGTH; i++) {
    lic_buffer[i * 2] = "0123456789ABCDEF"[lic_buffer[i + 32] >> 4];
    lic_buffer[i * 2 + 1] = "0123456789ABCDEF"[lic_buffer[i + 32] & 0x0f];
  }
  lic_buffer[BLOCK_LENGTH + 1] = 0;
}

/**
 * generate lic sn from niput data
 * @method lic_gen_sn
 * @param  input_data input data char array end with null
 * @return            pointer to ascii null array sn
 */
uint8_t *lic_gen_sn(char *input_data) {
  lic_engine.init();
  lic_engine.initHmac(lic_key, strlen((const char *)lic_key));
  // lic_engine.initHmac(lic_key, 16);
  lic_engine.print(input_data);
  memcpy(&lic_buffer[32], lic_engine.resultHmac(), HASH_LENGTH);
  lic_obsfuscate_raw_buffer();
  lic_raw_to_ascii();
  return lic_buffer;
}

#ifdef __cplusplus
}
#endif
#endif
