/**
 * @file mini_morse.cpp
 * @brief Compress a morse code of a letter in single byte.
 * @author Rishav
 * @date 2022/08/15
 */

#include "mini_morse.h"
#include "RH_RF24.h"

/* Start configuration */

// Morse transmission configurations
#define MINI_MORSE_RFM_TX_FREQUENCY 445
#define MINI_MORSE_WPM 20


// RFM pin configurations
#define MINI_MORSE_PIN_RFM_SDN 9
#define MINI_MORSE_PIN_RFM_SLAVE_SELECT 10
#define MINI_MORSE_PIN_RFM_INTERRUPT 2
#define MINI_MORSE_OUTPUT_PIN 7

/* End configuration */

#define MINI_MORSE_DIT_LEN (1200 / MINI_MORSE_WPM)
#define MINI_MORSE_DAH_LEN (MINI_MORSE_DIT_LEN * 3)
RH_RF24 radio(MINI_MORSE_PIN_RFM_SLAVE_SELECT, MINI_MORSE_PIN_RFM_INTERRUPT, MINI_MORSE_PIN_RFM_SDN);

RH_RF24::ModemConfig OOKAsync = {
    0x89, 0x00, 0xc3, 0x50, 0x01, 0x00, 0x00, 0x00, 0x00, 0x34,
    0x10, 0x00, 0x3f, 0x08, 0x31, 0x27, 0x04, 0x10, 0x02, 0x12,
    0x00, 0x2c, 0x03, 0xf9, 0x62, 0x11, 0x0e, 0x0e, 0x00, 0x02,
    0xff, 0xff, 0x00, 0x27, 0x00, 0x00, 0x07, 0xff, 0x40, 0xcc,
    0xa1, 0x30, 0xa0, 0x21, 0xd1, 0xb9, 0xc9, 0xea, 0x05, 0x12,
    0x11, 0x0a, 0x04, 0x15, 0xfc, 0x03, 0x00, 0xcc, 0xa1, 0x30,
    0xa0, 0x21, 0xd1, 0xb9, 0xc9, 0xea, 0x05, 0x12, 0x11, 0x0a,
    0x04, 0x15, 0xfc, 0x03, 0x00, 0x3f, 0x2c, 0x0e, 0x04, 0x0c,
    0x73};

struct MorseMapStruct
{
  const char letter;
  const uint8_t code;
};

static const MorseMapStruct morse_map[] PROGMEM = {
    // Alphabets
    {'A', (uint8_t)0b01000010}, // .-
    {'B', (uint8_t)0b10000100}, // -...
    {'C', (uint8_t)0b10100100}, // -.-.
    {'D', (uint8_t)0b10000011}, // -..
    {'E', (uint8_t)0b00000001}, // .
    {'F', (uint8_t)0b00100100}, // ..-.
    {'G', (uint8_t)0b11000011}, // --.
    {'H', (uint8_t)0b00000100}, // ....
    {'I', (uint8_t)0b00000010}, // ..
    {'J', (uint8_t)0b01111100}, // .---
    {'K', (uint8_t)0b10100011}, // -.-
    {'L', (uint8_t)0b01000100}, // .-..
    {'M', (uint8_t)0b11000010}, // --
    {'N', (uint8_t)0b10000010}, // -.
    {'O', (uint8_t)0b11100011}, // ---
    {'P', (uint8_t)0b01100100}, // .--.
    {'Q', (uint8_t)0b11010100}, // --.-
    {'R', (uint8_t)0b01000011}, // .-.
    {'S', (uint8_t)0b00000011}, // ...
    {'T', (uint8_t)0b10000001}, // -
    {'U', (uint8_t)0b00100011}, // ..-
    {'V', (uint8_t)0b00010100}, // ...-
    {'W', (uint8_t)0b01100011}, // .--
    {'X', (uint8_t)0b10010100}, // -..-
    {'Y', (uint8_t)0b10110100}, // -.--
    {'Z', (uint8_t)0b11000100}, // --..

    // Punctuations
    {' ', (uint8_t)0b00000000}, // "      "
    {',', (uint8_t)0b11001111}, // --..--
    {'.', (uint8_t)0b01010111}, // .-.-.-
    {'!', (uint8_t)0b10101111}, // -.-.--
    {':', (uint8_t)0b11100110}, // ---...
    {';', (uint8_t)0b10101110}, // -.-.-.
    {'(', (uint8_t)0b10110101}, // -.--.
    {')', (uint8_t)0b10110111}, // -.--.-
    {'"', (uint8_t)0b01001110}, // .-..-.
    {'@', (uint8_t)0b01101110}, // .--.-.
    {'&', (uint8_t)0b01000101}, // .-...
    {'?', (uint8_t)0b00110110}, // ..--..

    // Numbers
    {'1', (uint8_t)0b01111101}, // .----
    {'2', (uint8_t)0b00111101}, // ..---
    {'3', (uint8_t)0b00011101}, // ...--
    {'4', (uint8_t)0b00001101}, // ....-
    {'5', (uint8_t)0b00000101}, // .....
    {'6', (uint8_t)0b10000101}, // -....
    {'7', (uint8_t)0b11000101}, // --...
    {'8', (uint8_t)0b11100101}, // ---..
    {'9', (uint8_t)0b11110101}, // ----.
    {'0', (uint8_t)0b11111101}, // -----
};

uint8_t radio_init(void)
{
  pinMode(MINI_MORSE_PIN_RFM_SDN, OUTPUT);
  digitalWrite(MINI_MORSE_PIN_RFM_SDN, LOW);
  
  if (!radio.init())
  {
    return 0;
  }
  radio.setModemConfig(RH_RF24::GFSK_Rb0_5Fd1);
  radio.setCRCPolynomial(RH_RF24::CRC_CCITT);
  radio.setFrequency(MINI_MORSE_RFM_TX_FREQUENCY);
  radio.setModemRegisters(&OOKAsync);
  radio.setTxPower(0x7f);
  radio.setModeTx();

  return 1;
}

uint8_t minimorse_init(void)
{
  pinMode(MINI_MORSE_OUTPUT_PIN, OUTPUT);
  digitalWrite(MINI_MORSE_OUTPUT_PIN, LOW);
  uint8_t radio_status = radio_init();
  
  return radio_status;
}

void minimorse_transmit_msg(const String msg)
{
  radio.setModeTx();
  MorseMapStruct temp_morse_map{};
  for (uint8_t i = 0; i < msg.length(); i++)
  {
    for (uint8_t j = 0; j < sizeof(morse_map) / sizeof(*morse_map); j++)
    {
      memcpy_P(&temp_morse_map, &morse_map[j], sizeof(MorseMapStruct));
      if (toupper(msg[i]) == temp_morse_map.letter)
      {
        minimorse_parse_byte(temp_morse_map.code);
        break;
      }
    }
  }
  radio.setModeRx();
}


/**
 * @details
 *   if (morse_length < 6)
 *  {
 *      1. First 5 bits are for morse code; zeros (dit) and ones (dahs).
 *      2. Last three bits holds length of morse in binary.
 *      3. We are sure that XXX cannot be 110 and 111 as the morse length
 *         upto 5 is permitted.
 *  }
 *  else if (morse_length == 6)
 *  {
 *      1. This case is checked if second last and last bits are 1.
 *         It is to be noted that this condition is not possible in previous case.
 *      2. If we detect this 11 than all we do is say that k is the 6th morse code.
 *  }
 */
void minimorse_parse_byte(const uint8_t code)
{
  const char dit = '.';
  const char dah = '-';
  const char gap = ' ';

  uint8_t len = code & 7;
  if (len > 6)
  {
    len = 6;
  }

  if (len)
  {
    uint8_t cnt = 7 - len + (len == (uint8_t)6);
    for (uint8_t i = 7; i > cnt; i--)
    {
      if ((code >> i) & 1)
      {
        Serial.print(dah);
        minimorse_transmit_symbol(dah);
      }
      else
      {
        Serial.print(dit);
        minimorse_transmit_symbol(dit);
      }
    }
    if (len == 6)
    {
      if (code & 1)
      {
        Serial.print(dah);
        minimorse_transmit_symbol(dah);
      }
      else
      {
        Serial.print(dit);
        minimorse_transmit_symbol(dit);
      }
    }
  }
  else
  {
    for (uint8_t i = 0; i < 6; i++)
    {
      Serial.print(gap);
      minimorse_transmit_symbol(gap);
    }
  }
  Serial.print(gap);
  minimorse_transmit_symbol(gap);
}

inline void minimorse_transmit_symbol(const char s)
{
  switch (s)
  {
  case '.': // dit
  {
    digitalWrite(MINI_MORSE_OUTPUT_PIN, HIGH);
    delay(MINI_MORSE_DIT_LEN);
    digitalWrite(MINI_MORSE_OUTPUT_PIN, LOW);
    delay(MINI_MORSE_DIT_LEN);
    break;
  }

  case '-': // dah
  {
    digitalWrite(MINI_MORSE_OUTPUT_PIN, HIGH);
    delay(MINI_MORSE_DAH_LEN);
    digitalWrite(MINI_MORSE_OUTPUT_PIN, LOW);
    delay(MINI_MORSE_DIT_LEN);
    break;
  }
  case ' ': // gap
  {
    delay(MINI_MORSE_DIT_LEN);
    break;
  }
  }
}
