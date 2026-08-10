#include <assert.h>
#include <stdio.h>
#include "ring_buffer.h"

static void test_init_is_empty(void)
{
    uint8_t storage[8];
    ring_buffer_t rb;
    ringbuffer_init(&rb, storage, 8);

    assert(ringbuffer_is_empty(&rb));
    assert(!ringbuffer_is_full(&rb));
    assert(ringbuffer_count(&rb) == 0);
    printf("test_init_is_empty: PASS\n");
}

static void test_push_pop_order(void)
{
    uint8_t storage[8];
    ring_buffer_t rb;
    ringbuffer_init(&rb, storage, 8);

    assert(ringbuffer_push(&rb, 10));
    assert(ringbuffer_push(&rb, 20));
    assert(ringbuffer_push(&rb, 30));
    assert(ringbuffer_count(&rb) == 3);

    uint8_t v;
    assert(ringbuffer_pop(&rb, &v) && v == 10);
    assert(ringbuffer_pop(&rb, &v) && v == 20);
    assert(ringbuffer_pop(&rb, &v) && v == 30);
    assert(ringbuffer_is_empty(&rb));
    printf("test_push_pop_order: PASS\n");
}

static void test_fills_to_capacity_minus_one(void)
{
    /* an 8-slot buffer holds only 7 items; the 8th slot marks "full" vs "empty" */
    uint8_t storage[8];
    ring_buffer_t rb;
    ringbuffer_init(&rb, storage, 8);

    for (int i = 0; i < 7; i++) {
        assert(ringbuffer_push(&rb, (uint8_t)i));
    }
    assert(ringbuffer_is_full(&rb));
    assert(ringbuffer_count(&rb) == 7);

    assert(!ringbuffer_push(&rb, 99));
    assert(rb.overflow_count == 1);
    printf("test_fills_to_capacity_minus_one: PASS\n");
}

static void test_pop_from_empty_fails(void)
{
    uint8_t storage[4];
    ring_buffer_t rb;
    ringbuffer_init(&rb, storage, 4);

    uint8_t v;
    assert(!ringbuffer_pop(&rb, &v));
    printf("test_pop_from_empty_fails: PASS\n");
}

static void test_wraparound(void)
{
    uint8_t storage[4]; /* 3 usable slots */
    ring_buffer_t rb;
    ringbuffer_init(&rb, storage, 4);

    uint8_t v;

    /* fill, drain, refill repeatedly so head/tail wrap past the array end */
    for (int round = 0; round < 5; round++) {
        assert(ringbuffer_push(&rb, (uint8_t)(round * 10 + 1)));
        assert(ringbuffer_push(&rb, (uint8_t)(round * 10 + 2)));
        assert(ringbuffer_push(&rb, (uint8_t)(round * 10 + 3)));
        assert(!ringbuffer_push(&rb, 0xFF)); /* buffer only holds 3 */

        assert(ringbuffer_pop(&rb, &v) && v == (uint8_t)(round * 10 + 1));
        assert(ringbuffer_pop(&rb, &v) && v == (uint8_t)(round * 10 + 2));
        assert(ringbuffer_pop(&rb, &v) && v == (uint8_t)(round * 10 + 3));
        assert(ringbuffer_is_empty(&rb));
    }
    printf("test_wraparound: PASS\n");
}

static void test_flush_resets_indices(void)
{
    uint8_t storage[8];
    ring_buffer_t rb;
    ringbuffer_init(&rb, storage, 8);

    ringbuffer_push(&rb, 1);
    ringbuffer_push(&rb, 2);
    ringbuffer_flush(&rb);

    assert(ringbuffer_is_empty(&rb));
    assert(ringbuffer_count(&rb) == 0);
    printf("test_flush_resets_indices: PASS\n");
}

static void test_non_power_of_two_capacity_is_rounded_up(void)
{
    uint8_t storage[16];
    ring_buffer_t rb;
    ringbuffer_init(&rb, storage, 10); /* not a power of two */

    assert(rb.capacity == 16);
    assert(rb.mask == 15);
    printf("test_non_power_of_two_capacity_is_rounded_up: PASS\n");
}

int main(void)
{
    test_init_is_empty();
    test_push_pop_order();
    test_fills_to_capacity_minus_one();
    test_pop_from_empty_fails();
    test_wraparound();
    test_flush_resets_indices();
    test_non_power_of_two_capacity_is_rounded_up();

    printf("\nAll ring buffer tests passed.\n");
    return 0;
}
