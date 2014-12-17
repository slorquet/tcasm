BIN=tcasm
OBJS=main.o parser.o directives.o section.o chunk.o
OBJS+=arm.o

CC = gcc
CFLAGS = -g

$(BIN): $(OBJS)
	$(CC) -static $(OBJS) -o $@

clean:
	rm -f $(BIN)
	rm -f $(OBJS)

