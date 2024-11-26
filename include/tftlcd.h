#ifndef __spotify_buddy_hardware__
#define __spotify_buddy_hardware__

#include <stdint.h>
#include <stdbool.h>

// RGB Color struct
typedef struct _rgb_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_color;

uint16_t rgb_color_to_16bit(rgb_color color);
uint16_t color16bit(uint8_t r, uint8_t g, uint8_t b);


#define _TFT_LCD_DEFAULT_ID 0x9325
// TFT LCD struct
typedef struct _tft_lcd_struct {
    // gpio pins
    uint8_t cs;
    uint8_t cd;
    uint8_t wr;
    uint8_t rd;
} tftlcd;

// TFT LCD Control Functions
// TODO: parse from cpp file

#endif
