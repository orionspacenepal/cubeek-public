#ifndef _MINI_MORSE_H_
#define _MINI_MORSE_H_

#include <inttypes.h>
#include <Arduino.h>

uint8_t minimorse_init(void);
void minimorse_deinit(void);
void minimorse_transmit_msg(const String msg);
void minimorse_parse_byte(const uint8_t code);
inline void minimorse_transmit_symbol(const char s);

// 'Private' functions
// uint8_t radio_init(void);

#endif // mini_morse.h
