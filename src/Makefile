CC=gcc
BINS=server main.o builder.o accept.o

all: $(BINS)

clean:
	rm $(BINS)

server: builder.o accept.o connect.o main.o
	$(CC) -o server builder.o accept.o connect.o main.o

main.o: main.c builder.h
	$(CC) -c main.c

builder.o: builder.c builder.h
	$(CC) -c builder.c

accept.o: accept.c
	$(CC) -c accept.c

connect.o: connect.c
	$(CC) -c connect.c