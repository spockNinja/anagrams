CC=gcc
BINS=server main.o builder.o accept.o

all: $(BINS)

clean:
	rm $(BINS)

server: main.o builder.o accept.o
	$(CC) -o server main.o builder.o accept.o

main.o: main.c builder.h
	$(CC) -c main.c

builder.o: builder.c builder.h
	$(CC) -c builder.c

accept.o: accept.c
	$(CC) -c accept.c