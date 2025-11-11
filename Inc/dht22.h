/*
 * dht22.h
 * Simple DHT22 header for STM32 (blocking bitbanged read)
 *
 * Notes:
 * - Uses DWT cycle counter for microsecond delays (InitDWT in main)
 * - The read function is blocking and tuned for DHT22 timing
 */

#ifndef INC_DHT22_H_
#define INC_DHT22_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int dht22_read(float *temperature_c, float *humidity_pct);

/* Configure the DHT GPIO port/pin in your CubeMX-generated code.
   This driver expects HAL GPIO APIs to be available. */

#ifdef __cplusplus
}
#endif

#endif /* INC_DHT22_H_ */
