#include "tftlcd.h"
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>

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

void tftlcd_init(tftlcd *lcd, uint8_t cs, uint8_t dc, uint8_t sclk, uint8_t sdo, uint8_t sdi, uint8_t x, uint8_t y) {
    // set gpio pins
    lcd->cs = cs;
    lcd->dc = dc;
    lcd->sclk = sclk;
    lcd->sdo = sdo;
    lcd->sdi = sdi;
    lcd->x = x;
    lcd->y = y;

    // set GRAM defaults
    lcd->sp = 0x0000;
    lcd->sc = 0x0000;
    lcd->ep = TFTLCD_CMAX;
    lcd->ec = TFTLCD_PMAX;

    // initialize gpio
    gpio_init(cs);
    gpio_init(dc);
    gpio_init(sclk);
    gpio_init(sdo);
    gpio_init(sdi);
    adc_gpio_init(x);
    adc_gpio_init(y);

    // set pin directions
    gpio_set_dir(cs,   GPIO_OUT);
    gpio_set_dir(dc,   GPIO_OUT);
    gpio_set_dir(sclk, GPIO_OUT);
    gpio_set_dir(sdo, GPIO_OUT);
    gpio_set_dir(sdi, GPIO_IN);

    // set pulls
    gpio_pull_down(cs);
    gpio_pull_down(dc);
    gpio_pull_down(sclk);
    gpio_pull_down(sdo);
    gpio_pull_down(sdi);

    // set default state
    gpio_put(cs,   1);
    gpio_put(dc,   1);
    gpio_put(sclk, 0);
    gpio_put(sdo, 0);

    // run lcd startup
    _tftlcd_startup(lcd);
}

// Pulse LCD Clock pin at proper frequency
void _tftlcd_pulse_clock(tftlcd *lcd) {
    gpio_put(lcd->sclk, 1);
    sleep_us(TFTLCD_DELAY_US);
    gpio_put(lcd->sclk, 1);
    sleep_us(TFTLCD_DELAY_US);
}

// Startup sequence for LCD
void _tftlcd_startup(tftlcd* lcd) {
    // software reset
    tftlcd_write_cmd(lcd, HX8357_SWRESET);
    sleep_ms(10);

    // enable "extended command set" access
    // datasheet states SETEXTC must be followed by 0xFF, 0x83, 0x57 (in order)
    uint8_t setextc_args[] = {0xFF, 0x83, 0x57};
    tftlcd_write_cmd(lcd, HX8357_SETEXTC);
    tftlcd_write_args(lcd, setextc_args, 3);
    sleep_ms(300);

    // set RGB mode
    uint8_t setrgb_args[] = {
        0x80,   // Enable SDO pin on LCD, DBI mode,
        0x00,   // Default per datasheet
        0x06,   // Default per datasheet
        0x06    // Default per datasheet
    };
    tftlcd_write_cmd(lcd, HX8357_SETRGB);
    tftlcd_write_args(lcd, setrgb_args, 4);

    // Set VCOM Voltage (VCOM = -Vcc/2 = -1.65V --> datsheet --> 0x25)
    tftlcd_write_cmd(lcd, HX8357_SETCOM);
    tftlcd_write_arg(lcd, 0x25);

    // Set refresh rate: 60hz in NORMAL and IDLE 
    tftlcd_write_cmd(lcd, HX8357_SETOSC);
    tftlcd_write_arg(lcd, 0x66);

    // Set panel configs: DEFAULT
    //   * Normally black pixels
    //   * Color order = RGB
    //   * Normal gate driver
    //   * Normal source driver
    //   * Default SM_PANEL (0)
    tftlcd_write_cmd(lcd, HX8357_SETPANEL);
    tftlcd_write_arg(lcd, 0x00);

    // Set power config
    uint8_t setpwr_args[] = {
        0x00,   // no deep standby
        0x11,   // default voltage generation
        0x1C,   // default VSPR
        0x1C,   // default VSNR
        0x83,   // default AP and GASEN
        0x5C    // default frequency for voltage generation (adafruit used 0xAA? no idea what this is tbh.)
    };
    tftlcd_write_cmd(lcd, HX8357_SETPWR);
    tftlcd_write_args(lcd, setpwr_args, 6);

    // Set "source circuit option" (? datasheet does not elaborate...)
    uint8_t setstba_args[] = {
        0x50,   // set Normal Operation Period (similar to duty cycle of pixels?)
        0x50,   // set Idle Operation Period (similar to duty cycle of pixels?)
        0x00,   // default STBA[23:16]
        0x3C,   // default STBA[15:8]
        0xC4,   // default STBA[7:0]
        0x08    // default Gamma Operation Period (can ya guess what i was gunna type here?)
    };
    tftlcd_write_cmd(lcd, HX8357_SETSTBA);
    tftlcd_write_args(lcd, setstba_args, 6);

    // Set display cycle register (no clue what this is, using defaults);
    uint8_t setcyc_args[] = {
        0x02,
        0x40,
        0x00,
        0x2A,
        0x2A,
        0x0D,
        0x96
    };
    tftlcd_write_cmd(lcd, HX8357_SETCYC);
    tftlcd_write_args(lcd, setcyc_args, 6);

    // Set gamma curve (using default, not adafruit's)
    uint8_t setgamma_args[] = {
        0xE0,0x0B,0x0C,0x11,0x1D,0x25,0x37,0x43,
        0x4B,0x4E,0x47,0x41,0x39,0x35,0x31,0x2E,
        0x21,0x1C,0x1D,0x1D,0x26,0x31,0x44,0x4E,
        0x56,0x44,0x3F,0x39,0x33,0x31,0x2E,0x28,
        0x1D,0xE0, 0x00
    };
    tftlcd_write_cmd(lcd, HX8357_SETGAMMA);
    tftlcd_write_args(lcd, setgamma_args, 34);

    // Set Interface Pixel Format
    tftlcd_write_cmd(lcd, HX8357_COLMOD);
    // set 24-bit picture mode and 24-bit color mode
    tftlcd_write_arg(lcd, 0x77);

    // Set Memory Access Control config (p.g. 61 in datasheet for decoding)
    tftlcd_write_cmd(lcd, HX8357_MADCTL);
    tftlcd_write_arg(lcd, 0x00); // normal coord system
    //tftlcd_write_arg(lcd, 0xC0); // x-y exchange & y-invert

    // Set Tearing Effect Output off
    tftlcd_write_cmd(lcd, HX8357_TEON);
    tftlcd_write_arg(lcd, 0x00);
    
    // Set "Tearline" << no one knows what this is. using adafruit's value
    uint8_t tearline_args[] = {0x00, 0x02};
    tftlcd_write_cmd(lcd, HX8357_TEARLINE);
    tftlcd_write_args(lcd, tearline_args, 2);

    // Turn off sleep mode
    tftlcd_write_cmd(lcd, HX8357_SLPOUT);

    // Turn on display
    tftlcd_write_cmd(lcd, HX8357_DISPON);
}

// Set Data/Command pin LOW (a.k.a. COMMAND mode)
void _tftlcd_set_dc_low(tftlcd* lcd) {
    gpio_put(lcd->dc, 0);
    sleep_us(TFTLCD_DELAY_US);
}

// Set Data/Command pin HIGH (a.k.a. DATA mode)
void _tftlcd_set_dc_high(tftlcd* lcd) {
    gpio_put(lcd->dc, 1);
    sleep_us(TFTLCD_DELAY_US);
}

// Send byte over SDO; MSB first
void tftlcd_write(tftlcd* lcd, uint8_t byte) {
    gpio_put(lcd->sdo, byte & 0x80);
    _tftlcd_pulse_clock(lcd);
    gpio_put(lcd->sdo, byte & 0x40);
    _tftlcd_pulse_clock(lcd);
    gpio_put(lcd->sdo, byte & 0x20);
    _tftlcd_pulse_clock(lcd);
    gpio_put(lcd->sdo, byte & 0x10);
    _tftlcd_pulse_clock(lcd);
    gpio_put(lcd->sdo, byte & 0x08);
    _tftlcd_pulse_clock(lcd);
    gpio_put(lcd->sdo, byte & 0x04);
    _tftlcd_pulse_clock(lcd);
    gpio_put(lcd->sdo, byte & 0x02);
    _tftlcd_pulse_clock(lcd);
    gpio_put(lcd->sdo, byte & 0x01);
    _tftlcd_pulse_clock(lcd);
}

// Send command to LCD
void tftlcd_write_cmd(tftlcd* lcd, uint8_t byte) {
    // set Data/Command select low for COMMAND
    _tftlcd_set_dc_low(lcd);
    // send command byte
    tftlcd_write(lcd, byte);
}

// Send command argument to LCD
void tftlcd_write_arg(tftlcd* lcd, uint8_t byte) {
    // set Data/Command select high for DATA
    _tftlcd_set_dc_high(lcd);
    // send argument data byte
    tftlcd_write(lcd, byte);
}

// Send multiple command arguments to LCD
void tftlcd_write_args(tftlcd* lcd, uint8_t* args, uint8_t nargs) {
    // set Data/Command select high for DATA
    _tftlcd_set_dc_high(lcd);

    // send all argument bytes
    for (uint8_t i = 0; i < nargs; i++)
        tftlcd_write(lcd, args[i]);
}

// Send color argument to LCD
void tftlcd_write_color(tftlcd *lcd, rgb_color color) {
    // set Data/Command select high for DATA
    _tftlcd_set_dc_high(lcd);

    // send each byte of color
    tftlcd_write(lcd, color.r);
    tftlcd_write(lcd, color.g);
    tftlcd_write(lcd, color.b);
}

/**
 * @brief Set active GRAM window.
 * 
 * @param sp Starting Page Address
 * @param ep Ending Page Address (inclusive)
 * @param sc Starting Column Address
 * @param ec Ending Column Address (inclusive)
 */
void tftlcd_set_win_addr(tftlcd *lcd, uint16_t sp, uint16_t ep, uint16_t sc, uint16_t ec) {
    // update lcd struct's vars
    lcd->sp = sp;
    lcd->sc = sc;
    lcd->ep = ep;
    lcd->ec = ec;

    // set page address
    tftlcd_write_cmd(lcd, HX8357_PASET);
    tftlcd_write_arg(lcd, sp >> 8);
    tftlcd_write_arg(lcd, sp);
    tftlcd_write_arg(lcd, ep >> 8);
    tftlcd_write_arg(lcd, ep);

    // set column address
    tftlcd_write_cmd(lcd, HX8357_CASET);
    tftlcd_write_arg(lcd, sc >> 8);
    tftlcd_write_arg(lcd, sc);
    tftlcd_write_arg(lcd, ec >> 8);
    tftlcd_write_arg(lcd, ec);
}

void tftlcd_draw_pixel(tftlcd *lcd, uint16_t x, uint16_t y, rgb_color color) {
    tftlcd_set_win_addr(lcd, y, TFTLCD_H - 1, x, TFTLCD_W - 1);
    tftlcd_write_cmd(lcd, HX8357_RAMWR);
    tftlcd_write_color(lcd, color);
}

void tftlcd_fill_window(tftlcd* lcd, rgb_color color) {
    // reset address counter by re-setting current window address
    tftlcd_set_win_addr(lcd, lcd->sp, lcd->ep, lcd->sc, lcd->ec);
    // draw each pixel
    for (uint16_t page = lcd->sp; page <= lcd->ep; page++)
        for (uint16_t column = lcd->sc; column <= lcd->ec; column++)
            tftlcd_draw_pixel(lcd, column, page, color);
}

void tftlcd_fill_screen(tftlcd* lcd, rgb_color color) {
    // save current window
    uint16_t sp = lcd->sp;
    uint16_t sc = lcd->sc;
    uint16_t ep = lcd->ep;
    uint16_t ec = lcd->ec;

    // set window addr to full screen and fill
    tftlcd_set_win_addr(lcd, 0, TFTLCD_PMAX, 0, TFTLCD_CMAX);
    tftlcd_fill_window(lcd, color);

    // restore previous window
    tftlcd_set_win_addr(lcd, sp, ep, sc, ec);
}
