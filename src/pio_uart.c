/*
 * SPDX-License-Identifier: MIT
 *
 * The CDC bridge's UART on PIO (8n1), for boards whose receive pin is not a hardware UART RX pin
 * (board_tbd_config.h). It runs on pio1: pio0 holds the SWD program and autobaud.
 */
#include "board_config.h"

#ifdef PROBE_UART_PIO

#include "hardware/pio.h"
#include "pio_uart.h"
#include "pio_uart.pio.h"

static PIO const uart_pio = pio1;
static uint sm_tx, sm_rx, off_tx, off_rx;
static bool loaded;

uint32_t pio_uart_init(uint32_t baud) {
    if (!loaded) {
        off_tx = pio_add_program(uart_pio, &uart_tx_program);
        off_rx = pio_add_program(uart_pio, &uart_rx_program);
        sm_tx = pio_claim_unused_sm(uart_pio, true);
        sm_rx = pio_claim_unused_sm(uart_pio, true);
        loaded = true;
    }
    uart_tx_program_init(uart_pio, sm_tx, off_tx, PROBE_UART_TX, baud);
    uart_rx_program_init(uart_pio, sm_rx, off_rx, PROBE_UART_RX, baud);
    return baud;
}

void pio_uart_deinit(void) {
    pio_sm_set_enabled(uart_pio, sm_tx, false);
    pio_sm_set_enabled(uart_pio, sm_rx, false);
    pio_sm_clear_fifos(uart_pio, sm_tx);
    pio_sm_clear_fifos(uart_pio, sm_rx);
}

bool pio_uart_readable(void) {
    return !pio_sm_is_rx_fifo_empty(uart_pio, sm_rx);
}

char pio_uart_getc(void) {
    // The 8 bits arrive in the top byte of the shifted-right word.
    return (char)(pio_sm_get(uart_pio, sm_rx) >> 24);
}

void pio_uart_write_blocking(const uint8_t *src, size_t len) {
    for (size_t i = 0; i < len; i++)
        pio_sm_put_blocking(uart_pio, sm_tx, src[i]);
}

void pio_uart_set_break(bool en) {
    // A break holds the line low; the state machine stops while it is held and resumes idle-high.
    pio_sm_set_enabled(uart_pio, sm_tx, false);
    pio_sm_set_pins_with_mask(uart_pio, sm_tx, en ? 0u : (1u << PROBE_UART_TX), 1u << PROBE_UART_TX);
    if (!en)
        pio_sm_set_enabled(uart_pio, sm_tx, true);
}

#endif
