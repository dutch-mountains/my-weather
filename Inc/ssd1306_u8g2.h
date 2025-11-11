/*
 * ssd1306_u8g2.h
 * Simple u8g2 wrapper using HAL_I2C (hi2c1).
 * Requires u8g2 library sources (add u8g2 to project).
 *
 * Provides:
 *  - ssd1306_u8g2_init()
 *  - ssd1306_u8g2_get_u8g2() -> pointer to u8g2_t for drawing
 */

#ifndef INC_SSD1306_U8G2_H_
#define INC_SSD1306_U8G2_H_

#include "u8g2.h"

#ifdef __cplusplus
extern "C" {
#endif

void ssd1306_u8g2_init(void);
u8g2_t *ssd1306_u8g2_get_u8g2(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_SSD1306_U8G2_H_ */