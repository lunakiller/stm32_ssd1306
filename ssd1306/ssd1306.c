/*
 * ssd1306.c
 *
 *  Created on: Oct 3, 2025
 *      Author: lunakiller
 *         git: https://github.com/lunakiller/stm32_ssd1306
 */

#include "ssd1306.h"
#include <stdint.h>
#include <stdbool.h>


#define SSD1306_DISPLAY_ON 					0xAF
#define SSD1306_DISPLAY_OFF					0xAE
#define SSD1306_SEG_NORMAL					0xA0
#define SSD1306_SEG_REVERSE					0xA1
#define SSD1306_COLOR_NORMAL				0xA6
#define SSD1306_COLOR_INVERSE				0xA7
#define SSD1306_DISPLAY_DRAM				0xA4
#define SSD1306_DISPLAY_ALL_ON			0xA5
#define SSD1306_SET_MUX_RATIO				0xA8			// 16MUX-64MUX (15-63)
#define SSD1306_COM_NORMAL					0xC0
#define SSD1306_COM_REVERSE					0xC8
#define SSD1306_CHARGE_PUMP					0x8D			// 0x14 enable, 0x10 disable
#define SSD1306_SET_CONTRAST				0x81			// 0-255
#define SSD1306_SET_DISPLAY_OFFSET	0xD3			// 0-63
#define SSD1306_SET_COM_HW_CFG			0xDA			// 00xy0010; x - seq/alt COM pin cfg, y - dis/en COM L/R remap
#define SSD1306_SET_DCDR_OSC_FREQ		0xD9			// A[7:4] osc freq 0-15, A[3:0] disp clk divide ratio 1-15
#define SSD1306_SET_PRECHRG_PERIOD	0xD9			// A[7:4] phase2 1-15, A[3:0] phase1 1-15
#define SSD1306_SET_VCOM_DESEL_LVL	0xDB			// 0x00, 0x30, 0x30
#define SSD1306_NOP									0xE3

#define SSD1306_MEM_ADDR_MODE				0x20			// 0x0 - horizontal, 0x1 - vertical, 0x2 - page
#define SSD1306_PAGE_COL_ADDR_L			0x00			// + 0x0 - 0xF, page addressing mode
#define SSD1306_PAGE_COL_ADDR_H			0x10			// + 0x0 - 0xF, page addressing mode
#define SSD1306_SET_PAGE_START_ADDR	0xB0			// + 0x0 - 0x7 -> page0 - page7, page addressing mode
#define SSD1306_SET_COL_ADDR				0x21			// start 0-127, end 0-127, not page addressing mode
#define SSD1306_SET_PAGE_ADDR				0x22			// start 0-7, end 0-7, not page addressing mode
#define SSD1306_SET_START_LINE			0x40			// + line0 - line63


typedef struct {
		uint8_t curr_x;
		uint8_t curr_y;
} SSD1306_pos_t;


extern I2C_HandleTypeDef SSD1306_I2C_PORT;

static uint8_t SSD1306_buffer[SSD1306_BUFFER_SIZE];
static SSD1306_pos_t SSD1306;

// ----------------------------------------------------------------------------
void ssd1306_WriteCommand(uint8_t byte) {
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_ADDRESS, 0x00, 1, &byte, 1, SSD1306_TIMEOUT);
}

void ssd1306_WriteData(uint8_t* buffer, uint16_t size) {
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_ADDRESS, 0x40, 1, buffer, size, SSD1306_TIMEOUT);
}

// ----------------------------------------------------------------------------
void ssd1306_Init(void) {
	ssd1306_WriteCommand(SSD1306_DISPLAY_OFF);
	ssd1306_WriteCommand(SSD1306_MEM_ADDR_MODE);
	ssd1306_WriteCommand(0x0);													// horizontal addressing
	ssd1306_WriteCommand(SSD1306_SET_START_LINE);				// line 0
	ssd1306_WriteCommand(SSD1306_SEG_REVERSE);
	ssd1306_WriteCommand(SSD1306_COM_REVERSE);
	ssd1306_WriteCommand(SSD1306_SET_CONTRAST);
	ssd1306_WriteCommand(255);
	ssd1306_WriteCommand(SSD1306_SET_MUX_RATIO);
	ssd1306_WriteCommand(SSD1306_HEIGHT - 1);						// 64MUX
//	ssd1306_WriteCommand(SSD1306_SET_DISPLAY_OFFSET);
//	ssd1306_WriteCommand(0);														// reset default
	ssd1306_WriteCommand(SSD1306_SET_DCDR_OSC_FREQ);
	ssd1306_WriteCommand(0xF0);													// F_osc fastest, D=0
//	ssd1306_WriteCommand(SSD1306_SET_PRECHRG_PERIOD);
//	ssd1306_WriteCommand(0x22);													// reset default
//	ssd1306_WriteCommand(SSD1306_SET_COM_HW_CFG);
//	ssd1306_WriteCommand(0x12);													// reset default
//	ssd1306_WriteCommand(SSD1306_SET_VCOM_DESEL_LVL);
//	ssd1306_WriteCommand(0x20);													// reset default
	ssd1306_WriteCommand(SSD1306_DISPLAY_DRAM);
	ssd1306_WriteCommand(SSD1306_CHARGE_PUMP);
	ssd1306_WriteCommand(0x14);
	ssd1306_WriteCommand(SSD1306_DISPLAY_ON);

	ssd1306_Fill(PIXEL_OFF);
	ssd1306_Display();
}

void ssd1306_Fill(SSD1306_PixelState_t state) {
	uint8_t val = (state == PIXEL_ON) ? 0xFF : 0x00;

	for(uint16_t i = 0; i < SSD1306_BUFFER_SIZE; ++i) {
		SSD1306_buffer[i] = val;
	}
}

void ssd1306_Display(void) {
	ssd1306_WriteCommand(SSD1306_SET_COL_ADDR); 				// set column address
	ssd1306_WriteCommand(0); 														// start
	ssd1306_WriteCommand(127); 													// end
	ssd1306_WriteCommand(SSD1306_SET_PAGE_ADDR); 				// set page address
	ssd1306_WriteCommand(0); 														// start
	ssd1306_WriteCommand(7); 														// end
	ssd1306_WriteData(SSD1306_buffer, SSD1306_BUFFER_SIZE);
}

// default 0x20 = 32
void ssd1306_SetContrast(uint8_t val) {
	ssd1306_WriteCommand(SSD1306_SET_CONTRAST);
	ssd1306_WriteCommand(val);
}

void ssd1306_PowerSave(bool enable) {
	if(enable) {
		ssd1306_WriteCommand(SSD1306_DISPLAY_OFF);
		ssd1306_WriteCommand(SSD1306_DISPLAY_ALL_ON);
	}
	else {
		ssd1306_WriteCommand(SSD1306_DISPLAY_DRAM);
		ssd1306_WriteCommand(SSD1306_DISPLAY_ON);
	}
}

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_PixelState_t state) {
	if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
		return;
	}

	// Draw in the right color
	if(state == PIXEL_ON) {
		SSD1306_buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		SSD1306_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

void ssd1306_DrawVLine(uint8_t x, SSD1306_PixelState_t state) {
	if(state == PIXEL_ON) {
		for(int y = 0; y < SSD1306_PAGES; ++y) {
			SSD1306_buffer[x + y * SSD1306_WIDTH] |= 0xff;
		}
	}
	else {
		for(int y = 0; y < SSD1306_PAGES; ++y) {
			SSD1306_buffer[x + y * SSD1306_WIDTH] &= 0;
		}
	}
}

void ssd1306_DrawHLine(uint8_t y, SSD1306_PixelState_t state) {
	uint8_t val = 1 << (y % 8);
	if(state == PIXEL_ON) {
		for(int x = 0; x < SSD1306_WIDTH; ++x) {
			SSD1306_buffer[x + (y / 8) * SSD1306_WIDTH] |= val;
		}
	}
	else {
		for(int x = 0; x < SSD1306_WIDTH; ++x) {
			SSD1306_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(val);
		}
	}
}

void ssd1306_SetCursor(uint8_t x, uint8_t y) {
	SSD1306.curr_x = x;
	SSD1306.curr_y = y;
}

void ssd1306_WriteChar(char ch, fontStyle_t font, SSD1306_PixelState_t state) {
	ssd1306_WriteChar_FixedWidth(ch, font, 0, state);
}

// fixed_width = 0 -> variable-width
void ssd1306_WriteChar_FixedWidth(char ch, fontStyle_t font, uint8_t fixed_width, SSD1306_PixelState_t state) {
	// check if char is available in the font
	if(ch < font.FirstAsciiCode) {
		ch = 0;
	}
	else {
		ch -= font.FirstAsciiCode;
	}

	// variable-width/fixed-width
	uint8_t g_width;
	if(fixed_width == 0) {
		g_width = font.GlyphWidth[(int)ch];
	}
	else {
		g_width = fixed_width;
	}

	// check remaining space on the current line
	if (SSD1306_WIDTH < (SSD1306.curr_x + g_width) ||
		SSD1306_HEIGHT < (SSD1306.curr_y + font.GlyphHeight)) {
		// not enough space
		return;
	}

	uint32_t chr;

	for(uint32_t j = 0; j < font.GlyphHeight; ++j) {
		uint8_t width = g_width;

		for(uint32_t w = 0; w < font.GlyphBytesWidth; ++w) {
			chr = font.GlyphBitmaps[(ch * font.GlyphHeight + j) * font.GlyphBytesWidth + w];

			uint8_t w_range = width;
			if(w_range >= 8) {
				w_range = 8;
				width -= 8;
			}

			for(uint32_t i = 0; i < w_range; ++i) {
				if((chr << i) & 0x80)  {
					ssd1306_DrawPixel(SSD1306.curr_x + i + w*8, SSD1306.curr_y + j, state);
				} else {
					ssd1306_DrawPixel(SSD1306.curr_x + i + w*8, SSD1306.curr_y + j, !state);
				}
			}
		}
	}

	SSD1306.curr_x += g_width;

//	return ch + font.FirstAsciiCode;
}

void ssd1306_WriteString(const char *str, fontStyle_t font, SSD1306_PixelState_t state) {
	while(*str) {
		ssd1306_WriteChar(*str++, font, state);
	}
}

// fixed_width = 0 -> variable-width
void ssd1306_WriteString_FixedWidth(const char *str, fontStyle_t font, uint8_t fixed_width, SSD1306_PixelState_t state) {
	while(*str) {
		ssd1306_WriteChar_FixedWidth(*str++, font, fixed_width, state);
	}
}
