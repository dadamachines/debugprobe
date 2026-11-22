/*
 * TBD JIG Mini probe (RP2350-Zero U1 on the jig, or a Pico 2 for bench tests): same firmware as the TBD plug-in
 * probe, pins from the JIG Mini remap 2026-11-22 (tbd-hardware projects/tbd-16/boards/jig-mini/README.md):
 * SWCLK GP14, SWDIO GP15 (PIO side-set + in/out, must be consecutive), target reset GP11 (open-drain emulation),
 * TBD debug log (RP GPIO20 / U41 pin 4) received on GP13 by the PIO UART. GP12 is the (unused) UART TX.
 * Not yet in firmware: P4 UART TX on GP9, P4 enable on GP10.
 */
#ifndef BOARD_TBD_JIGMINI_CONFIG_H_
#define BOARD_TBD_JIGMINI_CONFIG_H_

#define PROBE_IO_RAW
#define PROBE_CDC_UART

#define PROBE_SM 0
#define PROBE_PIN_OFFSET 14
#define PROBE_PIN_SWCLK (PROBE_PIN_OFFSET + 0) // 14
#define PROBE_PIN_SWDIO (PROBE_PIN_OFFSET + 1) // 15
#define PROBE_PIN_RESET 11

#define PROBE_UART_PIO
#define PROBE_UART_RX 13
#define PROBE_UART_TX 12
#define PROBE_UART_INTERFACE uart1  // unused by the PIO UART
#define PROBE_UART_BAUDRATE 115200

#define PROBE_USB_CONNECTED_LED 25  // Pico 2 LED; the RP2350-Zero has no LED on GP25 (its RGB LED is on GP16, unused)

#define PROBE_PRODUCT_STRING "dadamachines TBD JIG Mini Probe (CMSIS-DAP)"

#endif
