CC=clang
CFLAGS = -Wall -Wextra -g
SRC = main.c
OBJ  = $(SRC:.c=.o)
BIN = bin

.PHONY: all clean

all: dirs build

dirs:
	mkdir -p ./$(BIN)

run: all
	$(BIN)/main

build: $(OBJ)
	$(CC) -o $(BIN)/main $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(BIN) $(OBJ)
