BIN=tcasm
SRCS=main.c parser.c directives.c section.c chunk.c
SRCS+=arm.c

OBJS=$(SRCS:.c=.o)

CC = gcc
CFLAGS = -g

.default: $(BIN)

$(BIN): Make.dep $(OBJS)
	$(CC) -static $(OBJS) -o $@

Make.dep: $(SRCS)
	$(CC) -MM $(SRCS) > $@

clean:
	rm -f $(BIN)
	rm -f $(OBJS)
	rm -f Make.dep

-include Make.dep

