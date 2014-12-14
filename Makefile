BIN=tcasm
OBJS=main.o parser.o directives.o

CC = gcc
CFLAGS = -g

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $@

clean:
	rm -f $(BIN)
	rm -f $(OBJS)

