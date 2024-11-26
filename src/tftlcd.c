#include "tftlcd.h"

// Get 16-bit representation of RGB color
uint16_t color16bit(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t c = 0x0000;

    // top 5 bits for red
    c |= (r & 0x1F) << 10;

    // middle 6 bits for green
    c |= (g & 0x2F) << 5;

    // last 5 bits for blue
    c |= (g & 0x1F);

    return c;
}

// Convert RGB Color to it's 16 bit representation
uint16_t rgb_color_to_16bit(rgb_color c) {
    return color16bit(c.r, c.g, c.b);
}

