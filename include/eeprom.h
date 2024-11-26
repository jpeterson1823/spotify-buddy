#ifndef __eeprom_25aa256__
#define __eeprom_25aa256__

#include <stdint.h>
#include <pico/stdlib.h>

#define EEPROM_DELAY_US 1

// Commands
#define EEPROM_WRSR  0b00000001 // "Write STATUS register"
#define EEPROM_READ  0b00000011 // "Read data from memory array beginning at selected address"
#define EEPROM_WRITE 0b00000010 // "Write data to memory array beginning at selected address"
#define EEPROM_WRDI  0b00000100 // "Reset the write enable latch (disable write operations)"
#define EEPROM_RDSR  0b00000101 // "Read STATUS register"
#define EEPROM_WREN  0b00000110 // "Set the write enable latch (enable write operations)"

// Microchip 25AA256 struct
typedef struct _microchip_25aa256_struct {
    uint8_t cs;
    uint8_t so;
    uint8_t si;
    uint8_t sclk;
    uint8_t wp;
} eeprom;

void eeprom_init(eeprom *prom, uint8_t wp, uint8_t cs, uint8_t si, uint8_t so, uint8_t sclk);
void _eeprom_cs_low(eeprom* prom);
void _eeprom_cs_high(eeprom* prom);
void _eeprom_send_byte(eeprom* prom, uint8_t byte);
void _eeprom_send_addr(eeprom* prom, uint16_t addr);
void _eeprom_pulse_sclk(eeprom* prom);
void eeprom_enable_write(eeprom* prom);
void eeprom_disable_write(eeprom* prom);
void eeprom_write_byte(eeprom* prom, uint16_t addr, uint8_t byte);
uint16_t eeprom_write_bytes(eeprom* prom, uint16_t addr, uint8_t* bytes, size_t n_bytes);
uint8_t eeprom_read_byte(eeprom* prom, uint16_t addr);

#endif
