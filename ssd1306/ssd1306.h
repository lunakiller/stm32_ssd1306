/*
 * ssd1306.h
 *  Library for SSD1306 based monochrome displays for STM32 MCUs.
 *
 *  Created on: Oct 3, 2025
 *      Author: lunakiller
 *         git: https://github.com/lunakiller/stm32_st7567a
 */

#ifndef __SSD1306_H
#define __SSD1306_H

#include "fontlibrary.h"
#include <stdbool.h>

/* ------------------------------- SETTINGS -------------------------------- */
// Include relevant ST HAL
#include "stm32g4xx_hal.h"

// Display parameters
#define SSD1306_WIDTH						128
#define SSD1306_HEIGHT					64
#define SSD1306_PAGES						(SSD1306_HEIGHT / 8)
#define SSD1306_BUFFER_SIZE   	(SSD1306_WIDTH * SSD1306_HEIGHT / SSD1306_PAGES)

// I2C settings
#define SSD1306_ADDRESS					(0x3C << 1)
#define SSD1306_I2C_PORT 				hi2c1
#define SSD1306_TIMEOUT  				100

// enum for "color"
typedef enum {
	PIXEL_OFF = 0x0,
	PIXEL_ON = 0x1
} SSD1306_PixelState_t;


/* ------------------------------- FUNCTIONS ------------------------------- */
void ssd1306_Init(void);
void ssd1306_Fill(SSD1306_PixelState_t state);
void ssd1306_Display(void);

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_PixelState_t state);
void ssd1306_DrawVLine(uint8_t x, SSD1306_PixelState_t state);
void ssd1306_DrawHLine(uint8_t y, SSD1306_PixelState_t state);

void ssd1306_SetCursor(uint8_t x, uint8_t y);
void ssd1306_WriteChar(char ch, fontStyle_t font, SSD1306_PixelState_t state);
void ssd1306_WriteString(const char *str, fontStyle_t font, SSD1306_PixelState_t state);
void ssd1306_WriteChar_FixedWidth(char ch, fontStyle_t font, uint8_t fixed_width, SSD1306_PixelState_t state);
void ssd1306_WriteString_FixedWidth(const char *str, fontStyle_t font, uint8_t fixed_width, SSD1306_PixelState_t state);

void ssd1306_SetContrast(uint8_t val);
void ssd1306_PowerSave(bool enable);

// low-level functions
void ssd1306_WriteCommand(uint8_t byte);
void ssd1306_WriteData(uint8_t* buffer, uint16_t size);


#endif /* __SSD1306_H */

