# Ring Buffer UART Console

A bare-metal-style ring buffer project: a lock-free single-producer /
single-consumer byte ring buffer, used to build a small UART command
console the way it would actually be wired up in firmware.

No OS, no threads. The "producer" is a simulated interrupt handler
(`uart_rx_isr`), the "consumer" is the main super-loop. This is the same
architecture used for a real UART/serial driver: the ISR pushes each
received byte into the ring buffer, and the main loop drains it whenever
it gets a chance.

## Layout

```
inc/ring_buffer.h        ring buffer API
src/ring_buffer.c        ring buffer implementation
src/main.c               UART console demo (LED ON/OFF, STATUS, STRESS, HELP)
tests/test_ring_buffer.c unit tests (push/pop order, wraparound, overflow, flush)
Makefile
```

## Build & run

```
make run     # build and run the console demo
make test    # build and run the unit tests
make clean
```

## Design notes

- **Power-of-two capacity.** `head`/`tail` wrap with `& mask` instead of
  `% capacity`, which is cheap enough to use inside an ISR.
- **One slot is always wasted.** A `capacity`-byte buffer holds
  `capacity - 1` bytes; the last slot distinguishes "full" from "empty"
  without a separate counter that both sides would need to touch.
- **No locks needed.** Because there is exactly one writer (`head`,
  written only by the producer) and one reader (`tail`, written only by
  the consumer), and `head` is only published *after* the byte is
  stored, the buffer is safe between an ISR and the main loop with no
  `cli()/sei()` critical section — as long as `head`/`tail` reads and
  writes are atomic on the target (true for 8/16/32-bit indices on
  virtually all MCUs).
- **Overflow is counted, not fatal.** `ringbuffer_push()` returns `false`
  and increments `overflow_count` when full, mirroring how a UART
  peripheral just drops bytes on a receiver overrun — the demo's
  `STRESS` command shows this: flooding a 32-byte buffer with 300 bytes
  while nothing drains it drops 269.

## Porting to real hardware

Replace the simulated `uart_rx_isr()` call site with the real interrupt
vector, e.g. on an STM32:

```c
void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_RXNE) {
        uint8_t byte = (uint8_t)USART1->DR;
        ringbuffer_push(&rx_rb, byte);
    }
}
```

and call `ringbuffer_pop(&rx_rb, &byte)` from the main loop exactly as
`main.c` does here.
