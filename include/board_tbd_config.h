/*
 * The TBD-16 direct plug-in probe: a standard Pico or Pico 2 with a straight 1x5 header in its physical
 * pins 3-7 (GND, GP2, GP3, GP4, GP5), plugged straight into the TBD-16's 6-pin debug/extension port
 * (TBD pin 1 GND, 2 SWCLK, 3 SWDAT, 4 the TBD's debug UART TX, 5 optional return). TBD pin 6 (+5 V) stays free.
 *
 * GP4 receives and GP5 transmits, the reverse of the RP2040/RP2350 hardware UART pin functions
 * (GP4 can only be UART1 TX), so the CDC bridge runs on a PIO UART (src/pio_uart.c) instead.
 */
#ifndef BOARD_TBD_CONFIG_H_
#define BOARD_TBD_CONFIG_H_

#define PROBE_IO_RAW
#define PROBE_CDC_UART

// PIO config
#define PROBE_SM 0
#define PROBE_PIN_OFFSET 2
#define PROBE_PIN_SWCLK (PROBE_PIN_OFFSET + 0) // 2
#define PROBE_PIN_SWDIO (PROBE_PIN_OFFSET + 1) // 3

// UART config: a PIO UART on pio1 (pio0 holds SWD and autobaud)
#define PROBE_UART_PIO
#define PROBE_UART_RX 4
#define PROBE_UART_TX 5
#define PROBE_UART_INTERFACE uart1  // unused by the PIO UART; kept for the shared code's signatures
#define PROBE_UART_BAUDRATE 115200

#define PROBE_USB_CONNECTED_LED 25

#define PROBE_PRODUCT_STRING "dadamachines TBD Debug Probe (CMSIS-DAP)"

#endif
