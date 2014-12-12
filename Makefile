BIN=tcasm
OBJS=main.o

CC = gcc
CFLAGS = -g

$(BIN): $(OBJS)
	$(CC) $< -o $@

clean:
	rm -f $(BIN)
	rm -f $(OBJS)

