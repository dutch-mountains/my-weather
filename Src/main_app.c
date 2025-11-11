/*
 * main_app.c
 * Application logic: CMSIS-RTOS2 tasks for sensor and display
 *
 * This file is intended to be used inside the CubeMX-generated project.
 * Place it in Src/ and include in build. Make sure to:
 *  - call InitDWT() once after HAL_Init()/SystemClock_Config()
 *  - call MX_I2C1_Init() and MX_GPIO_Init() (CubeMX generated)
 */

#include "cmsis_os2.h"
#include "dht22.h"
#include "ssd1306_u8g2.h"
#include "u8g2.h"
#include <stdio.h>
#include <math.h>

/* Shared data */
typedef struct {
  float temperature;
  float humidity;
  float dewpoint;
} sensor_data_t;

static sensor_data_t g_sensor_data = {0};
static osMutexId_t data_mutex;

/* DWT delay prototype - implement in main.c if not present */
extern void InitDWT(void);
extenr void delay_us(uint32_t us);

/* dew point (Magnus) */
static float dewpoint_magnus(float T, float RH) {
  const double a = 17.27;
  const double b = 237.7;
  double alpha = (a * T) / (b + T) + log(RH / 100.0);
  double Td = (b * alpha) / (a - alpha);
  return (float)Td;
}

/* Sensor task */
void sensor_task(void *argument) {
  (void)argument;
  for (;;) {
    float T = 0, RH = 0;
    if (dht22_read(&T, &RH) == 0) {
      float dew = dewpoint_magnus(T, RH);
      if (osMutexAcquire(data_mutex, 100) == osOK) {
        g_sensor_data.temperature = T;
        g_sensor_data.humidity = RH;
        g_sensor_data.dewpoint = dew;
        osMutexRelease(data_mutex);
      }
    }
    osDelay(3000); /* DHT22 minimum ~2s; use 3s */
  }
}

/* Display task */
void display_task(void *argument) {
  (void)argument;
  ssd1306_u8g2_init();
  u8g2_t *u8g2 = ssd1306_u8g2_get_u8g2();
  char buf[64];

  for (;;) {
    sensor_data_t snap;
    if (osMutexAcquire(data_mutex, 100) == osOK) {
      snap = g_sensor_data;
      osMutexRelease(data_mutex);
    }
    /* Draw using u8g2 */
    u8g2_ClearBuffer(u8g2);
    snprintf(buf, sizeof(buf), "T: %.1f C", snap.temperature);
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(u8g2, 0, 12, buf);
    snprintf(buf, sizeof(buf), "RH: %.1f %%", snap.humidity);
    u8g2_DrawStr(u8g2, 0, 28, buf);
    snprintf(buf, sizeof(buf), "Dew: %.1f C", snap.dewpoint);
    u8g2_DrawStr(u8g2, 0, 44, buf);
    u8g2_SendBuffer(u8g2);

    osDelay(500);
  }
}

/* Application init (call this once after HAL init and before osKernelStart) */
void app_init(void) {
  /* Init DWT for microsecond delays */
  InitDWT();

  /* Create mutex */
  const osMutexAttr_t mutex_attr = { .name = "dataMutex" };
  data_mutex = osMutexNew(&mutex_attr);

  /* Create tasks */
  const osThreadAttr_t sensor_attr = { .name = "Sensor", .priority = osPriorityAboveNormal, .stack_size = 1024 };
  const osThreadAttr_t display_attr = { .name = "Display", .priority = osPriorityNormal, .stack_size = 1024 };

  osThreadNew(sensor_task, NULL, &sensor_attr);
  osThreadNew(display_task, NULL, &display_attr);
}