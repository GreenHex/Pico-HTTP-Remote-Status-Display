# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# set(PICO_BOARD pico_w CACHE STRING "Board type")
set(PICO_BOARD pico_w)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ble_wifi C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_compile_definitions(LWIP_DEBUG)

add_subdirectory(lib/Config)
add_subdirectory(lib/Infrared)
add_subdirectory(lib/Icm20948)
add_subdirectory(lib/LCD)
add_subdirectory(lib/Fonts)
add_subdirectory(lib/GUI)
# add_subdirectory(flash)
# add_subdirectory(ble)
# add_subdirectory(src)

add_executable(${PROJECT_NAME}
	./lib/Config/DEV_Config.c
	./lib/Fonts/font8.c
	./lib/Fonts/font12.c
	./lib/Fonts/font16.c
	./lib/Fonts/font20.c
	./lib/Fonts/font24.c
	./lib/Fonts/font48.c
	./lib/Fonts/font50.c
	./lib/GUI/GUI_Paint.c
	./lib/Icm20948/icm20948.c
	./lib/Infrared/Infrared.c
	./lib/LCD/LCD_1in3.c
	./tiny-json/tiny-json.c
	./tiny-json/json_proc.c
	./keys/check_keys.c
	./http_client/http_client.c
	./tcp_client/tcp_client.c
	./flash/flash.c
	./ble/ble.c
	./not_resolved_list/not_resolved_list.c
	./src/main.c
	)

target_compile_options(${PROJECT_NAME} INTERFACE -g -Werror -Wall -Wextra -Wpedantic -Wnull-dereference)

pico_btstack_make_gatt_header(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/wifi_provisioning.gatt")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE .)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_lwip_threadsafe_background
	pico_cyw43_driver
	pico_cyw43_arch
	hardware_flash
	hardware_sync
    pico_lwip_http
	LCD
	Infrared
	Icm20948
	GUI
	Fonts
	Config
	hardware_spi
	pico_multicore
	pico_bootsel_via_double_reset
    )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_add_extra_outputs(${PROJECT_NAME})

# pico_set_program_name(${PROJECT_NAME} "ble_wifi01")
# pico_set_program_version(${PROJECT_NAME} "0.0")
