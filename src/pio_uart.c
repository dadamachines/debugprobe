/*
 * SPDX-License-Identifier: MIT
 *
 * The CDC bridge's UART on PIO (8n1), for boards whose receive pin is not a hardware UART RX pin
 * (board_tbd_config.h). It runs on pio1: pio0 holds the SWD program and autobaud.
 */
#include "board_config.h"

#ifdef PROBE_UART_PIO

#include "hardware/irq.h"
#include "hardware/pio.h"
#include "pio_uart.h"
#include "pio_uart.pio.h"

static PIO const uart_pio = pio1;
static uint sm_tx, sm_rx, off_tx, off_rx;
static bool loaded;

// The receive FIFO holds 8 bytes (0.7 ms at 115200); the bridge task polls more slowly, and a full FIFO
// stalls the receiver into mid-byte. An interrupt drains it into this ring as each byte arrives.
#define RX_RING_SIZE 4096u
static volatile uint8_t rx_ring[RX_RING_SIZE];
static volatile uint32_t rx_head, rx_tail;

static void pio_uart_rx_irq(void) {
    while (!pio_sm_is_rx_fifo_empty(uart_pio, sm_rx)) {
        const uint8_t c = (uint8_t)(pio_sm_get(uart_pio, sm_rx) >> 24);
        const uint32_t next = (rx_head + 1u) % RX_RING_SIZE;
        if (next != rx_tail) {  // ring full: drop the newest byte, never stall the receiver
            rx_ring[rx_head] = c;
            rx_head = next;
        }
    }
}

uint32_t pio_uart_init(uint32_t baud) {
    if (!loaded) {
        off_tx = pio_add_program(uart_pio, &uart_tx_program);
        off_rx = pio_add_program(uart_pio, &uart_rx_program);
        sm_tx = pio_claim_unused_sm(uart_pio, true);
        sm_rx = pio_claim_unused_sm(uart_pio, true);
        irq_set_exclusive_handler(PIO1_IRQ_0, pio_uart_rx_irq);
        irq_set_enabled(PIO1_IRQ_0, true);
        loaded = true;
    }
    uart_tx_program_init(uart_pio, sm_tx, off_tx, PROBE_UART_TX, baud);
    uart_rx_program_init(uart_pio, sm_rx, off_rx, PROBE_UART_RX, baud);
    pio_set_irq0_source_enabled(uart_pio, pio_get_rx_fifo_not_empty_interrupt_source(sm_rx), true);
    return baud;
}

void pio_uart_deinit(void) {
    pio_set_irq0_source_enabled(uart_pio, pio_get_rx_fifo_not_empty_interrupt_source(sm_rx), false);
    pio_sm_set_enabled(uart_pio, sm_tx, false);
    pio_sm_set_enabled(uart_pio, sm_rx, false);
    pio_sm_clear_fifos(uart_pio, sm_tx);
    pio_sm_clear_fifos(uart_pio, sm_rx);
}

bool pio_uart_readable(void) {
    return rx_head != rx_tail;
}

char pio_uart_getc(void) {
    while (rx_head == rx_tail)
        tight_loop_contents();
    const char c = (char)rx_ring[rx_tail];
    rx_tail = (rx_tail + 1u) % RX_RING_SIZE;
    return c;
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
