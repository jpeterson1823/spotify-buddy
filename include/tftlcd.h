#ifndef __spotify_buddy_hardware__
#define __spotify_buddy_hardware__

#include <stdint.h>
#include <stdbool.h>

#define TFTLCD_DELAY_US 1
#define TFTLCD_W 320u
#define TFTLCD_H 480u
#define TFTLCD_CMAX 0x013Fu     // 319
#define TFTLCD_PMAX 0x01DFu     // 479

// TFT LCD controller commands (courtesy of Adafruit: https://github.com/adafruit/Adafruit_HX8357_Library/blob/master/Adafruit_HX8357.h)
// Command names have been shortened. Project uses HX8357D
#define HX8357_NOP       0x00   ///< No op
#define HX8357_SWRESET   0x01   ///< software reset
#define HX8357_RDDID     0x04   ///< Read ID
#define HX8357_RDDST     0x09   ///< (unknown)
#define HX8357_RDPOWMODE 0x0A   ///< Read power mode Read power mode
#define HX8357_RDMADCTL  0x0B   ///< Read MADCTL
#define HX8357_RDCOLMOD  0x0C   ///< Column entry mode
#define HX8357_RDDIM     0x0D   ///< Read display image mode
#define HX8357_RDDSDR    0x0F   ///< Read dosplay signal mode
#define HX8357_SLPIN     0x10   ///< Enter sleep mode
#define HX8357_SLPOUT    0x11   ///< Exit sleep mode
#define HX8357_INVOFF    0x20   ///< Turn off invert
#define HX8357_INVON     0x21   ///< Turn on invert
#define HX8357_DISPOFF   0x28   ///< Display on
#define HX8357_DISPON    0x29   ///< Display off
#define HX8357_CASET     0x2A   ///< Column addr set
#define HX8357_PASET     0x2B   ///< Page addr set
#define HX8357_RAMWR     0x2C   ///< Write VRAM
#define HX8357_RAMRD     0x2E   ///< Read VRAm
#define HX8357_TEON      0x35   ///< Tear enable on
#define HX8357_TEARLINE  0x44   ///< (unknown)
#define HX8357_MADCTL    0x36   ///< Memory access control
#define HX8357_COLMOD    0x3A   ///< Color mode
#define HX8357_SETOSC    0xB0   ///< Set oscillator
#define HX8357_SETPWR    0xB1   ///< Set power control
#define HX8357_SETRGB    0xB3   ///< Set RGB interface
#define HX8357_SETCOM    0xB6   ///< Set VCOM voltage
#define HX8357_SETCYC    0xB4   ///< Set display cycle reg
#define HX8357_SETEXTC   0xB9   ///< Enable extension command
#define HX8357_SETSTBA   0xC0   ///< Set source option
#define HX8357_SETPANEL  0xCC   ///< Set Panel
#define HX8357_SETGAMMA  0xE0   ///< Set Gamma

// Colors provided in same Adafruit header
#define HX8357_BLACK    0x0000  ///< BLACK color for drawing graphics
#define HX8357_BLUE     0x001F  ///< BLUE color for drawing graphics
#define HX8357_RED      0xF800  ///< RED color for drawing graphics
#define HX8357_GREEN    0x07E0  ///< GREEN color for drawing graphics
#define HX8357_CYAN     0x07FF  ///< CYAN color for drawing graphics
#define HX8357_MAGENTA  0xF81F  ///< MAGENTA color for drawing graphics
#define HX8357_YELLOW   0xFFE0  ///< YELLOW color for drawing graphics
#define HX8357_WHITE    0xFFFF  ///< WHITE color for drawing graphics

// RGB Color struct
typedef struct _rgb_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_color;
uint16_t rgb_color_to_16bit(rgb_color color);
uint16_t color16bit(uint8_t r, uint8_t g, uint8_t b);

// TFT LCD struct
typedef struct _tft_lcd_struct {
    //gpio pins
    uint8_t cs;
    uint8_t dc;
    uint8_t sclk;
    uint8_t sdo;
    uint8_t sdi;
    uint8_t x;
    uint8_t y;

    // GRAM address info
    uint16_t sp, ep;
    uint16_t sc, ec;
} tftlcd;

// TFT LCD Functions
void tftlcd_init(
    tftlcd* lcd,
    uint8_t cs,
    uint8_t dc, 
    uint8_t sclk, 
    uint8_t sdo,
    uint8_t sdi,
    uint8_t x,
    uint8_t y
);

void tftlcd_write(tftlcd* lcd, uint8_t byte);
void tftlcd_write_cmd(tftlcd* lcd, uint8_t byte);
void tftlcd_write_arg(tftlcd* lcd, uint8_t arg);
void tftlcd_write_args(tftlcd* lcd, uint8_t* args, uint8_t nargs);
void tftlcd_write_color(tftlcd* lcd, rgb_color color);
void tftlcd_set_win_addr(tftlcd* lcd, uint16_t sp, uint16_t ep, uint16_t sc, uint16_t ec);

void tftlcd_draw_pixel(tftlcd* lcd, uint16_t x, uint16_t y, rgb_color color);
void tftlcd_fill_window(tftlcd* lcd, rgb_color color);
void tftlcd_fill_screen(tftlcd* lcd, rgb_color color);

void _tftlcd_pulse_clock(tftlcd* lcd);
void _tftlcd_set_dc_low(tftlcd* lcd);
void _tftlcd_set_dc_high(tftlcd* lcd);
void _tftlcd_startup(tftlcd* lcd);

#endif
