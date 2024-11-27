// Pico SDK
#include <pico/stdio.h>
#include <pico/cyw43_arch.h>        // Pico W wireless
#include <hardware/adc.h>
#include <stdio.h>

// lwIP
//#include "lwip/dns.h"               // Hostname resolution
//#include "lwip/altcp_tls.h"         // TCP + TLS (+ HTTP == HTTPS)
//#include "lwip/prot/iana.h"         // HTTPS port number

// Pico HTTPS request example
//extern "C" {
//    #include "picohttps.h" // courtesy of https://github.com/marceloalcocer/picohttps/tree/main 
//}

#include "tftlcd.h"




int main()
{
    stdio_init_all();
    cyw43_arch_init();
    adc_init();

    // sleep for 5 seconds to allow for serial connection
    sleep_ms(5000);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    tftlcd lcd;
    printf("Initializing TFTLCD... ");
    tftlcd_init(&lcd, 20, 19, 16, 18, 17, 27, 26);
    printf("DONE\nFilling screen... ");
    tftlcd_fill_screen(&lcd, (rgb_color){0, 165, 255});
    printf("DONE\n");

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}
