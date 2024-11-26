#include "eeprom.h"

#include <pico/stdlib.h>
#include <stdio.h>
#include <hardware/gpio.h>

// Initialize EEPROM. Must have defined pins in provided struct.
void eeprom_init(eeprom *prom, uint8_t wp, uint8_t cs, uint8_t si, uint8_t so, uint8_t sclk) {
    // store prom gpio pin
    prom->wp = wp;
    prom->cs = cs;
    prom->si = si;
    prom->so = so;
    prom->sclk = sclk;

    // initialize GPIO pins
    printf("Init GPIO pins\n");
    gpio_init(prom->wp);
    gpio_init(prom->cs);
    gpio_init(prom->so);
    gpio_init(prom->si);
    gpio_init(prom->sclk);

    // set gpio direction
    printf("Setting GPIO direction\n");
    gpio_set_dir(prom->wp, GPIO_OUT);
    gpio_set_dir(prom->cs, GPIO_OUT);
    gpio_set_dir(prom->si, GPIO_OUT);
    gpio_set_dir(prom->so, GPIO_IN);
    gpio_set_dir(prom->sclk, GPIO_OUT);

    // set pulls
    printf("Setting GPIO pulls\n");
    gpio_pull_up(prom->wp);
    gpio_pull_up(prom->cs);
    //gpio_pull_down(prom->so);
    gpio_pull_down(prom->si);
    gpio_pull_down(prom->sclk);

    // set default state
    printf("Setting default GPIO state\n");
    gpio_put(prom->wp, 1);
    gpio_put(prom->cs, 1);
    gpio_put(prom->si, 0);
    gpio_put(prom->sclk, 0);
}

// Drive CS pin LOW with following delay
void _eeprom_cs_low(eeprom* prom) {
    gpio_put(prom->cs, 0);
    sleep_us(EEPROM_DELAY_US);
}

// Drive CS pin HIGH with following delay
void _eeprom_cs_high(eeprom* prom) {
    gpio_put(prom->cs, 1);
    sleep_us(EEPROM_DELAY_US);
}

// Pulse serial clock
void _eeprom_pulse_sclk(eeprom* prom) {
    gpio_put(prom->sclk, 1);
    sleep_us(EEPROM_DELAY_US);
    gpio_put(prom->sclk, 0);
    sleep_us(EEPROM_DELAY_US);
}

// Send byte to EEPROM; MSB first
void _eeprom_send_byte(eeprom* prom, uint8_t byte) {
    gpio_put(prom->si, byte & 0x80);
    _eeprom_pulse_sclk(prom);

    gpio_put(prom->si, byte & 0x40);
    _eeprom_pulse_sclk(prom);

    gpio_put(prom->si, byte & 0x20);
    _eeprom_pulse_sclk(prom);

    gpio_put(prom->si, byte & 0x10);
    _eeprom_pulse_sclk(prom);

    gpio_put(prom->si, byte & 0x08);
    _eeprom_pulse_sclk(prom);

    gpio_put(prom->si, byte & 0x04);
    _eeprom_pulse_sclk(prom);

    gpio_put(prom->si, byte & 0x02);
    _eeprom_pulse_sclk(prom);

    gpio_put(prom->si, byte & 0x01);
    _eeprom_pulse_sclk(prom);
}

// Send 16-bit address to eeprom; MSB first
void _eeprom_send_addr(eeprom* prom, uint16_t addr) {
    // send 16-bit address
    gpio_put(prom->si, addr & 0x8000);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x4000);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x2000);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x1000);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0800);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0400);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0200);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0100);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0080);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0040);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0020);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0010);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0008);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0004);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0002);
    _eeprom_pulse_sclk(prom);
    gpio_put(prom->si, addr & 0x0001);
    _eeprom_pulse_sclk(prom);
}

// Read byte at specific address
uint8_t eeprom_read_byte(eeprom* prom, uint16_t addr) {
    // drive cs low
    _eeprom_cs_low(prom);

    // send READ instruction
    _eeprom_send_byte(prom, EEPROM_READ);

    // send addr
    _eeprom_send_addr(prom, addr);

    // read byte output
    uint8_t byte = 0x00;
    byte |= (uint8_t)(gpio_get(prom->so)) << 7;
    _eeprom_pulse_sclk(prom);
    byte |= (uint8_t)(gpio_get(prom->so)) << 6;
    _eeprom_pulse_sclk(prom);
    byte |= (uint8_t)(gpio_get(prom->so)) << 5;
    _eeprom_pulse_sclk(prom);
    byte |= (uint8_t)(gpio_get(prom->so)) << 4;
    _eeprom_pulse_sclk(prom);
    byte |= (uint8_t)(gpio_get(prom->so)) << 3;
    _eeprom_pulse_sclk(prom);
    byte |= (uint8_t)(gpio_get(prom->so)) << 2;
    _eeprom_pulse_sclk(prom);  
    byte |= (uint8_t)(gpio_get(prom->so)) << 1;
    _eeprom_pulse_sclk(prom);
    byte |= (uint8_t)(gpio_get(prom->so));
    _eeprom_pulse_sclk(prom);

    // drive cs high
    _eeprom_cs_high(prom);
    sleep_ms(6);

    // return read byte
    return byte;
}

// Write byte to specified address
void eeprom_write_byte(eeprom* prom, uint16_t addr, uint8_t byte) {
    // set write enable latch
    eeprom_enable_write(prom);
    sleep_us(EEPROM_DELAY_US);

    // drive cs low
    _eeprom_cs_low(prom);

    // send WRITE instruction
    _eeprom_send_byte(prom, EEPROM_WRITE);

    // send address
    _eeprom_send_addr(prom, addr);

    // send data byte
    _eeprom_send_byte(prom, byte);

    // drive cs high
    _eeprom_cs_high(prom);
    //sleep_us(EEPROM_DELAY_US);
    sleep_ms(6);
}

// Write byte array to eeprom. Returns address immediately after last written byte
uint16_t eeprom_write_bytes(eeprom *prom, uint16_t addr, uint8_t *bytes, size_t n_bytes) {
    // loop until all bytes are written (bytes written in increments of 64)
    while (n_bytes > 0) {
        // set write enable latch
        eeprom_enable_write(prom);

        // drive cs low
        _eeprom_cs_low(prom);

        // send write instruction
        _eeprom_send_byte(prom, EEPROM_WRITE);

        // send address
        _eeprom_send_addr(prom, addr);

        // send byte stream (max 64 bytes)
        uint8_t i = 0;
        for (; i < 64 && n_bytes > 0; i++, n_bytes--) {
            _eeprom_send_byte(prom, bytes[i]);
        }

        // drive cs high
        _eeprom_cs_high(prom);

        // move write address forwared by number of bytes written
        addr += i + 1;
    }

    // return address after last written byte
    return addr;
}

void eeprom_enable_write(eeprom* prom) {
    _eeprom_cs_low(prom);
    _eeprom_send_byte(prom, EEPROM_WREN);
    _eeprom_cs_high(prom);
}

void eeprom_disable_write(eeprom* prom) {
    _eeprom_cs_low(prom);
    _eeprom_send_byte(prom, EEPROM_WRDI);
    _eeprom_cs_high(prom);
}
