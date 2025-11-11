/*
 * dht22.c
 * Minimal DHT22 driver for STM32 HAL.
 *
 * Returns 0 on success, -1 on error.
 *
 * IMPORTANT:
 * - Configure the DHT pin in CubeMX (we use PA0 by default).
 * - Ensure InitDWT() is called once before using dht22_read() so delay_us works.
 */

#include "dht22.h"
#include "main.h" /* for HAL_GPIO_ReadPin / HAL_GPIO_WritePin and DHT pin defs */
#include "stm32f4xx_hal.h"
#include <string.h>

/* Edit these to match your board wiring if different */
#ifndef DHT_PORT
#define DHT_PORT GPIOA
#endif
#ifndef DHT_PIN
#define DHT_PIN GPIO_PIN_0
#endif

/* Prototype for delay in microseconds - implemented in main (InitDWT + delay_us) */
extern void delay_us(uint32_t us);

/* Helper to switch GPIO mode: output push-pull or input floating */
static void dht_set_output(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_DeInit(DHT_PORT, DHT_PIN);
  HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

static void dht_set_input(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_DeInit(DHT_PORT, DHT_PIN);
  HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

int dht22_read(float *temperature_c, float *humidity_pct) {
  uint8_t data[5] = {0};
  uint32_t t;
  int i;

  /* Start signal: pull low for >= 1 ms (we use 1.2 ms) */
  dht_set_output();
  HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_RESET);
  delay_us(1200);

  /* Pull high and switch to input */
  HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);
  delay_us(30);
  dht_set_input();

  /* Wait for sensor response (low ~80us) */
  t = 0;
  while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_SET) {
    delay_us(1);
    if (++t >= 1000) return -1;
  }
  /* Wait for sensor's high (~80us) */
  t = 0;
  while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_RESET) {
    delay_us(1);
    if (++t >= 1000) return -1;
  }
  /* Wait for first data low */
  t = 0;
  while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_SET) {
    delay_us(1);
    if (++t >= 1000) return -1;
  }

  /* Read 40 bits */
  for (i = 0; i < 40; ++i) {
    /* wait for low (50us) */
    t = 0;
    while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_RESET) {
      delay_us(1);
      if (++t > 1000) return -1;
    }
    /* measure length of the following high */
    uint32_t len = 0;
    while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_SET) {
      delay_us(1);
      len++;
      if (len > 200) break;
    }
    data[i / 8] <<= 1;
    /* threshold: > ~40us high means '1' */
    if (len > 40) data[i / 8] |= 1;
  }

  /* Restore pin to output-high idle */
  dht_set_output();
  HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);

  uint8_t checksum = data[0] + data[1] + data[2] + data[3];
  if (checksum != data[4]) return -1;

  uint16_t rawH = (data[0] << 8) | data[1];
  uint16_t rawT = (data[2] << 8) | data[3];

  *humidity_pct = rawH / 10.0f;
  if (rawT & 0x8000) {
    rawT &= 0x7FFF;
    *temperature_c = - (rawT / 10.0f);
  } else {
    *temperature_c = rawT / 10.0f;
  }

  return 0;
}