CC = clang
CFLAGS = -Wall -Wextra -std=c23 -g
BIN = bin
TARGET = $(BIN)/main
SRC = main.c

all: dirs $(TARGET)

dirs:
	mkdir -p $(BIN)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -rf $(BIN)

.PHONY: all dirs clean
