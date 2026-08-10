#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Lock-free single-producer / single-consumer byte ring buffer.
 *
 * Intended firmware usage:
 *   - ringbuffer_push() is called from an ISR (e.g. UART RX interrupt).
 *   - ringbuffer_pop()  is called from the main super-loop.
 *
 * Safe without disabling interrupts because there is exactly one writer
 * (head) and one reader (tail), and each index is only ever written by
 * its own side. Capacity must be a power of two so wraparound is a mask
 * instead of a modulo, keeping the ISR path fast and branch-free.
 */

typedef struct {
    uint8_t *buffer;
    uint16_t capacity;                /* power of two */
    uint16_t mask;                    /* capacity - 1 */
    volatile uint16_t head;           /* next write index (producer-owned) */
    volatile uint16_t tail;           /* next read index  (consumer-owned) */
    volatile uint32_t overflow_count; /* bytes dropped because buffer was full */
} ring_buffer_t;

void     ringbuffer_init(ring_buffer_t *rb, uint8_t *storage, uint16_t capacity);
bool     ringbuffer_push(ring_buffer_t *rb, uint8_t data);
bool     ringbuffer_pop(ring_buffer_t *rb, uint8_t *data);
bool     ringbuffer_is_empty(const ring_buffer_t *rb);
bool     ringbuffer_is_full(const ring_buffer_t *rb);
uint16_t ringbuffer_count(const ring_buffer_t *rb);
void     ringbuffer_flush(ring_buffer_t *rb);

#endif /* RING_BUFFER_H */
