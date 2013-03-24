CC=gcc
BINS=server main.o builder.o

all: $(BINS)

clean:
	rm $(BINS)

server: main.o builder.o
	$(CC) -o server main.o builder.o

main.o: main.c builder.h
	$(CC) -c main.c

builder.o: builder.c builder.h
	$(CC) -c builder.c

