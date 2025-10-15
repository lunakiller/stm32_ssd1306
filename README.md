# stm32_ssd1306
Library for SSD1306 based monochrome displays for STM32 MCUs. 
Based on STM32 HAL libraries and [ST7567A library](https://github.com/lunakiller/stm32_st7567a). 

Tested with generic 0.96" 128x64 I2C OLED Display on WeAct STM32G431CoreBoard.

Right now the library supports I2C only.

## Features
- framebuffer-based
- basic graphic primitives
    - point
    - horintal line (fast)
    - vertical line (fast)
- fixed-width or variable-width fonts
- power-save procedure
- example project
    - build with Makefile or STM32CubeIDE

## How-to
- include the correct HAL header for your MCU ([#L18](https://github.com/lunakiller/stm32_ssd1306/blob/74bcd6f1da05fe6daba53638dd523e516a2c1d4e/ssd1306/ssd1306.h#L18))
- initialize I2C with HAL, provide the handle name and set OLED address ([#L27-L28](https://github.com/lunakiller/stm32_ssd1306/blob/74bcd6f1da05fe6daba53638dd523e516a2c1d4e/ssd1306/ssd1306.h#L27-L28))
- include `ssd1306.h` in your project

## Fonts
The library supports variable-width fonts. Two free pixel fonts are included:
- [Dogica](https://www.dafont.com/dogica.font) (8px)
- [RetroVille NC](https://www.dafont.com/retroville-nc.font) (9px)
- [videotype](https://www.dafont.com/videotype.font) (18px)

More fonts can be generated using BMFont and slightly patched `bmfont2c.py` script by [Lars Ole Pontoppidan](https://larsee.com/blog/2014/05/converting-fonts-to-c-source-using-bmfont2c/). Instructions can be found in [stm32_st7567a/fonts/README.md](https://github.com/lunakiller/stm32_st7567a/blob/main/fonts/README.md).

## to-do
- [ ] Partial screen refresh
- [ ] Arbitrary lines


### Source
- [SSD1306 Datasheet rev 1.1](https://www.crystalfontz.com/controllers/SolomonSystech/SSD1306/)
- [dafont.com](https://www.dafont.com/)
