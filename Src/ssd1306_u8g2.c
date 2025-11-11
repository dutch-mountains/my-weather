/*
 * ssd1306_u8g2.c
 * u8g2 HAL-I2C transport implementation for STM32 HAL (I2C1, address 0x3C).
 *
 * This file implements u8g2's byte callback and rotation init.
 * You must add the u8g2 sources (u8g2.c, fonts, etc.) to the project.
 */

#include "ssd1306_u8g2.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include <string.h>

/* Default I2C handle created by CubeMX */
extern I2C_HandleTypeDef hi2c1;

/* u8g2 object */
static u8g2_t u8g2;

/* I2C address */
#define SSD1306_ADDR (0x3C << 1)

/* u8g2 byte callback for I2C */
uint8_t u8g2_byte_stm32_hal_i2c(u8g2_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  switch (msg) {
    case U8G2_MSG_BYTE_INIT:
      /* nothing to init here, HAL I2C configured by CubeMX */
      return 1;
    case U8G2_MSG_BYTE_START_TRANSFER:
      return 1;
    case U8G2_MSG_BYTE_SEND:
      {
        uint8_t *buf = (uint8_t *)arg_ptr;
        if (arg_int == 0) return 1;
        /* Prepend control byte 0x40 for data or 0x00 for commands is handled in u8g2 */
        HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR, buf, arg_int, 1000);
        return (st == HAL_OK) ? 1 : 0;
      }
    case U8G2_MSG_BYTE_END_TRANSFER:
      return 1;
  }
  return 0;
}

void ssd1306_u8g2_init(void) {
  /* Initialize u8g2 for SSD1306 128x64 I2C, using U8G2_R0 rotation.
     Requires u8g2 setup call from u8g2 library; example: u8g2_Setup_ssd1306_i2c_128x64_noname_f()
     NOTE: adjust the setup function if you use a different controller. */
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8g2_byte_stm32_hal_i2c, u8g2_gpio_and_delay_stub);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
}

u8g2_t *ssd1306_u8g2_get_u8g2(void) {
  return &u8g2;
}