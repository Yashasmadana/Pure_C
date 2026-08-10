CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Iinc
SRC := src/ring_buffer.c src/main.c
BIN := ringbuffer_demo
TEST_SRC := src/ring_buffer.c tests/test_ring_buffer.c
TEST_BIN := test_ring_buffer

.PHONY: all run test clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN)

test: $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(BIN) $(BIN).exe $(TEST_BIN) $(TEST_BIN).exe
