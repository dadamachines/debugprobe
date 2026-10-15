/* A PIO UART with the hardware UART's call shapes, for boards that define PROBE_UART_PIO. */
#ifndef PIO_UART_H_
#define PIO_UART_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint32_t pio_uart_init(uint32_t baud);
// 8n1 only: other formats are accepted and ignored.
static inline void pio_uart_set_format(unsigned data_bits, unsigned stop_bits, int parity) {
    (void)data_bits; (void)stop_bits; (void)parity;
}
void pio_uart_deinit(void);
bool pio_uart_readable(void);
char pio_uart_getc(void);
void pio_uart_write_blocking(const uint8_t *src, size_t len);
void pio_uart_set_break(bool en);

#endif
