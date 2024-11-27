# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)
project(spotify-buddy C CXX ASM)

pico_sdk_init()

add_executable(spotify-buddy
    spotify-buddy.c

    #src/picohttps.c
    src/tftlcd.c
    src/eeprom.c
)

pico_set_program_name(spotify-buddy "spotify-buddy")
pico_set_program_version(spotify-buddy "0.1")

pico_enable_stdio_uart(spotify-buddy 0)
pico_enable_stdio_usb(spotify-buddy 1)

target_include_directories(spotify-buddy PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ./include
)

target_link_libraries(spotify-buddy 
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_mbedtls
    pico_lwip_mbedtls
    hardware_adc
)

pico_add_extra_outputs(spotify-buddy)

# Add compile-time definitions
target_compile_definitions(spotify-buddy
    PRIVATE WIFI_SSID=\"$ENV{PICO_WIFI_SSID}\"
    PRIVATE WIFI_PSK=\"$ENV{PICO_WIFI_PSK}\"
)
