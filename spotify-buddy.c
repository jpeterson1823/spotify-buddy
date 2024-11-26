// Pico SDK
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"        // Pico W wireless

// lwIP
//#include "lwip/dns.h"               // Hostname resolution
//#include "lwip/altcp_tls.h"         // TCP + TLS (+ HTTP == HTTPS)
//#include "lwip/prot/iana.h"         // HTTPS port number

// Pico HTTPS request example
//extern "C" {
//    #include "picohttps.h" // courtesy of https://github.com/marceloalcocer/picohttps/tree/main 
//}

#include "eeprom.h"

void eeprom_test(eeprom* prom) {
    printf("Running eeprom_test...");

    uint8_t byte0 = 0x65;
    uint8_t byte1 = 0x43;
    uint8_t byte2 = 0x21;

    printf("Write Test:\n");
    printf("    Write %#04x to 0x0000\n", byte0);
    printf("    Write %#04x to 0x0001\n", byte1);
    printf("    Write %#04x to 0x0002\n", byte2);

    // write to eeprom
    printf("Beginning writing... ");
    eeprom_write_byte(prom, 0x0000, byte0);
    eeprom_write_byte(prom, 0x0001, byte1);
    eeprom_write_byte(prom, 0x0002, byte2);
    printf("DONE\n");

    sleep_us(1000);

    printf("Read Test:\n");
    printf("    Read 0x0000\n");
    printf("    Read 0x0001\n");
    printf("    Read 0x0002\n");

    // read from eeprom
    printf("Beginning reading... ");
    byte0 = eeprom_read_byte(prom, 0x0000);
    byte1 = eeprom_read_byte(prom, 0x0001);
    byte2 = eeprom_read_byte(prom, 0x0002);
    printf("DONE\n");

    printf("Results of read test:\n");
    printf("    0x0000 : %#04x\n", byte0);
    printf("    0x0001 : %#04x\n", byte1);
    printf("    0x0002 : %#04x\n", byte2);
    printf(" DONE!\n");
    fflush(stdout);
}




int main()
{
    stdio_init_all();
    cyw43_arch_init();
    sleep_ms(5000);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    printf("Creating eeprom object... ");
    eeprom prom;
    eeprom_init(&prom, 2, 0, 4, 1, 3);
    printf("DONE\n");

    eeprom_test(&prom);

    printf("Checking problematic address:\n");
    printf("0x0001 : %#04x\n", eeprom_read_byte(&prom, 0x0001));

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}
