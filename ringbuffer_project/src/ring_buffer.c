#include "ring_buffer.h"

static bool is_power_of_two(uint16_t n)
{
    return n != 0 && (n & (n - 1)) == 0;
}

void ringbuffer_init(ring_buffer_t *rb, uint8_t *storage, uint16_t capacity)
{
    if (!is_power_of_two(capacity)) {
        uint16_t rounded = 1;
        while (rounded < capacity) {
            rounded = (uint16_t)(rounded << 1);
        }
        capacity = rounded;
    }

    rb->buffer = storage;
    rb->capacity = capacity;
    rb->mask = (uint16_t)(capacity - 1);
    rb->head = 0;
    rb->tail = 0;
    rb->overflow_count = 0;
}

bool ringbuffer_push(ring_buffer_t *rb, uint8_t data)
{
    uint16_t head = rb->head;
    uint16_t next = (uint16_t)((head + 1) & rb->mask);

    if (next == rb->tail) {
        rb->overflow_count++;
        return false; /* full: caller (ISR) drops the byte */
    }

    rb->buffer[head] = data;
    rb->head = next; /* publish last so the consumer never sees a half-written slot */
    return true;
}

bool ringbuffer_pop(ring_buffer_t *rb, uint8_t *data)
{
    uint16_t tail = rb->tail;

    if (tail == rb->head) {
        return false; /* empty */
    }

    *data = rb->buffer[tail];
    rb->tail = (uint16_t)((tail + 1) & rb->mask);
    return true;
}

bool ringbuffer_is_empty(const ring_buffer_t *rb)
{
    return rb->head == rb->tail;
}

bool ringbuffer_is_full(const ring_buffer_t *rb)
{
    return (uint16_t)((rb->head + 1) & rb->mask) == rb->tail;
}

uint16_t ringbuffer_count(const ring_buffer_t *rb)
{
    return (uint16_t)((rb->head - rb->tail) & rb->mask);
}

void ringbuffer_flush(ring_buffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
}
