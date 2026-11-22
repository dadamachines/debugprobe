cmake_minimum_required(VERSION 3.12)

# build for the Raspberry Pi Debug Probe by default
set(PICO_BOARD debug_probe CACHE STRING "Board type")

set(CMAKE_BUILD_TYPE RelWithDebInfo)

include(pico_sdk_import.cmake)

set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/freertos)
include(FreeRTOS_Kernel_import.cmake)

project(debugprobe)

pico_sdk_init()

add_executable(debugprobe
        src/probe_config.c
        src/main.c
        src/usb_descriptors.c
        src/probe.c
        src/cdc_uart.c
        src/get_serial.c
        src/sw_dp_pio.c
        src/tusb_edpt_handler.c
        src/autobaud.c
        src/pio_uart.c
)

target_sources(debugprobe PRIVATE
        CMSIS_DAP/CMSIS/DAP/Firmware/Source/DAP.c
        CMSIS_DAP/CMSIS/DAP/Firmware/Source/JTAG_DP.c
        CMSIS_DAP/CMSIS/DAP/Firmware/Source/DAP_vendor.c
        CMSIS_DAP/CMSIS/DAP/Firmware/Source/SWO.c
        #CMSIS_DAP/CMSIS/DAP/Firmware/Source/SW_DP.c
        )

target_include_directories(debugprobe PRIVATE
        CMSIS_DAP/CMSIS/DAP/Firmware/Include/
        CMSIS_DAP/CMSIS/Core/Include/
        include/
        )

target_compile_options(debugprobe PRIVATE -Wall)

pico_generate_pio_header(debugprobe ${CMAKE_CURRENT_LIST_DIR}/src/probe.pio)
pico_generate_pio_header(debugprobe ${CMAKE_CURRENT_LIST_DIR}/src/probe_oen.pio)
pico_generate_pio_header(debugprobe ${CMAKE_CURRENT_LIST_DIR}/src/autobaud.pio)
pico_generate_pio_header(debugprobe ${CMAKE_CURRENT_LIST_DIR}/src/pio_uart.pio)

target_include_directories(debugprobe PRIVATE src)

# add version
add_custom_target(version
        ${CMAKE_SOURCE_DIR}/get-version.sh
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(debugprobe PRIVATE
        ${CMAKE_BINARY_DIR}/generated
        )

add_dependencies(debugprobe version)

target_compile_definitions (debugprobe PRIVATE
	PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1
    PICO_ENABLE_USB_RESET_VIA_VENDOR_INTERFACE=1
)

# The TBD-16 direct plug-in probe on a standard Pico or Pico 2 (include/board_tbd_config.h):
# cmake -DPICO_BOARD=pico2 -DDEBUGPROBE_TBD=ON  ->  tbd-debugprobe-pico2.uf2
option(DEBUGPROBE_TBD "Build the TBD-16 direct plug-in pin mapping" OFF)

option(DEBUGPROBE_TBD_JIGMINI "Build the TBD JIG Mini pin mapping (include/board_tbd_jigmini_config.h)" OFF)

if (DEBUGPROBE_TBD_JIGMINI)
    target_compile_definitions(debugprobe PRIVATE TBD_JIGMINI_DEBUGPROBE=1)
    set_target_properties(debugprobe PROPERTIES
        OUTPUT_NAME "tbd-jigmini-debugprobe-${PICO_BOARD}"
    )
elseif (DEBUGPROBE_TBD)
    target_compile_definitions(debugprobe PRIVATE TBD_DEBUGPROBE=1)
    set_target_properties(debugprobe PROPERTIES
        OUTPUT_NAME "tbd-debugprobe-${PICO_BOARD}"
    )
elseif (NOT PICO_BOARD STREQUAL "debug_probe")
    set_target_properties(debugprobe PROPERTIES
        OUTPUT_NAME "debugprobe_on_${PICO_BOARD}"
    )
endif ()

target_link_libraries(debugprobe PRIVATE
        pico_multicore
        pico_stdlib
        pico_usb_reset
        pico_unique_id
        tinyusb_device
        tinyusb_board
        hardware_pio
        hardware_dma
        hardware_irq
        hardware_clocks
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap1
)

pico_set_binary_type(debugprobe copy_to_ram)

pico_add_extra_outputs(debugprobe)
