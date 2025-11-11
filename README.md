# my-weather — STM32 Nucleo-F401RE dauwpuntsensor (FreeRTOS)

Dit is een STM32CubeIDE/FreeRTOS voorbeeldproject voor Nucleo-F401RE dat:
- DHT22 uitleest (bitbanged met DWT microsecond delays)
- dauwpunt berekent (Magnus-formule)
- waarden toont op een SSD1306 OLED via u8g2 (I2C)

Branch: feature/stm32-dewpoint

Wiring
- DHT22:
  - VCC -> 3.3V
  - GND -> GND
  - DATA -> PA0 (GPIO, met 4.7k-10k pull-up naar 3.3V)
- SSD1306 (I2C):
  - VCC -> 3.3V
  - GND -> GND
  - SDA -> PB7
  - SCL -> PB6
  - I2C adres: 0x3C

CubeMX / STM32CubeIDE setup (kort)
1. Nieuw project: Nucleo-F401RE.
2. System Clock: HSI with PLL to 84 MHz (instellingen in het .ioc bestand).
3. Peripherals:
   - I2C1: Enable, set PB6=SCL, PB7=SDA.
   - Configure PA0 als GPIO Output (CubeMX kan later als Input/Output worden aangepast in code).
4. Middleware:
   - FreeRTOS: enable (CMSIS‑RTOS2). Kies default config.
5. Generate code naar STM32CubeIDE.
6. Kopieer de files uit deze branch in de passende src/inc mappen van het project en voeg u8g2 library toe (zie hieronder).

u8g2 integratie
- Voeg de u8g2 library toe als submodule of plaats de benodigde u8g2 bestanden in Drivers/u8g2/.
- De wrapper ssd1306_u8g2.c gebruikt HAL_I2C via callback; zorg dat hi2c1 beschikbaar is (gegenereerd door CubeMX).

Bestanden in deze branch
- Inc/dht22.h
- Src/dht22.c
- Inc/ssd1306_u8g2.h
- Src/ssd1306_u8g2.c
- Src/main_app.c
- my-weather.ioc

Build & Run
1. Importeer project in STM32CubeIDE.
2. Voeg u8g2-source files toe (of voeg u8g2 als submodule/library).
3. Build en flash naar Nucleo-F401RE.
4. Open serial voor debug (optioneel) of bekijk OLED.