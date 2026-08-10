/*
 * Ring buffer application demo: a UART-style command console.
 *
 * Architecture mirrors a real firmware super-loop:
 *   - uart_rx_isr()   = the producer. In real firmware this is the
 *                       USARTx_IRQHandler, called once per received byte.
 *   - main() loop      = the consumer. Runs forever, draining whatever
 *                       bytes the ISR has queued and acting on complete
 *                       lines ("commands").
 *
 * There is no OS and no threads: the ISR and the main loop are the two
 * sides of a single-producer/single-consumer ring buffer, exactly as in
 * bare-metal firmware. Here the "interrupt" is simulated by calling
 * uart_rx_isr() directly instead of it firing asynchronously on real
 * hardware.
 */

#include <stdio.h>
#include <string.h>
#include "ring_buffer.h"

#define RX_BUFFER_CAPACITY 32u /* must be a power of two; 31 bytes usable */
#define LINE_MAX 64u

static uint8_t rx_storage[RX_BUFFER_CAPACITY];
static ring_buffer_t rx_rb;

static uint32_t bytes_received = 0;
static uint32_t commands_executed = 0;
static bool led_state = false;

/* ---- hardware layer (stands in for the real UART RX interrupt) ---- */
static void uart_rx_isr(uint8_t byte)
{
    bytes_received++;
    ringbuffer_push(&rx_rb, byte); /* full buffer just drops the byte, like real hardware overrun */
}

/* Floods the buffer faster than it can be drained, to show overflow counting. */
static void simulate_overflow_burst(void)
{
    const int burst_size = 300;
    printf("  [ISR] flooding %d bytes into a %u-byte buffer with nobody reading...\n",
           burst_size, (unsigned)rx_rb.capacity);

    for (int i = 0; i < burst_size; i++) {
        uart_rx_isr((uint8_t)('A' + (i % 26)));
    }

    printf("  [ISR] burst done -> overflow_count = %lu (bytes silently dropped)\n",
           (unsigned long)rx_rb.overflow_count);

    /* the burst is noise, not a command line: discard it so it doesn't corrupt framing */
    uint8_t discard;
    while (ringbuffer_pop(&rx_rb, &discard)) {
        /* intentionally empty */
    }
}

/* ---- application layer: tiny command interpreter ---- */
static void handle_command(const char *line)
{
    commands_executed++;

    if (strcmp(line, "LED ON") == 0) {
        led_state = true;
        printf("  -> LED turned ON\n");
    } else if (strcmp(line, "LED OFF") == 0) {
        led_state = false;
        printf("  -> LED turned OFF\n");
    } else if (strcmp(line, "STATUS") == 0) {
        printf("  -> LED=%s  bytes_received=%lu  overflow=%lu  commands=%lu\n",
               led_state ? "ON" : "OFF",
               (unsigned long)bytes_received,
               (unsigned long)rx_rb.overflow_count,
               (unsigned long)commands_executed);
    } else if (strcmp(line, "STRESS") == 0) {
        simulate_overflow_burst();
    } else if (strcmp(line, "HELP") == 0) {
        printf("  -> commands: LED ON | LED OFF | STATUS | STRESS | HELP\n");
    } else {
        printf("  -> unknown command: \"%s\"\n", line);
    }
}

int main(void)
{
    ringbuffer_init(&rx_rb, rx_storage, RX_BUFFER_CAPACITY);

    /* scripted "incoming serial data", one console command per line */
    const char *script[] = {
        "HELP",
        "LED ON",
        "STATUS",
        "STRESS",
        "STATUS",
        "LED OFF",
        "STATUS",
    };
    const int script_len = (int)(sizeof(script) / sizeof(script[0]));

    char line[LINE_MAX];
    int line_pos = 0;

    printf("=== Ring buffer UART console demo (bare-metal super-loop, no OS/threads) ===\n");
    printf("RX ring buffer capacity = %u bytes\n\n", (unsigned)rx_rb.capacity);

    for (int cmd = 0; cmd < script_len; cmd++) {
        printf("[tx] \"%s\"\n", script[cmd]);

        /* producer: one uart_rx_isr() call per byte, exactly like real UART hardware */
        for (const char *p = script[cmd]; *p != '\0'; p++) {
            uart_rx_isr((uint8_t)*p);
        }
        uart_rx_isr((uint8_t)'\n');

        /* consumer: main loop drains whatever the ISR queued this "tick" */
        uint8_t byte;
        while (ringbuffer_pop(&rx_rb, &byte)) {
            if (byte == '\n') {
                line[line_pos] = '\0';
                handle_command(line);
                line_pos = 0;
            } else if (line_pos < (int)LINE_MAX - 1) {
                line[line_pos++] = (char)byte;
            }
        }
    }

    printf("\n=== Final stats ===\n");
    printf("bytes_received   : %lu\n", (unsigned long)bytes_received);
    printf("bytes_dropped    : %lu\n", (unsigned long)rx_rb.overflow_count);
    printf("commands_executed: %lu\n", (unsigned long)commands_executed);

    return 0;
}
