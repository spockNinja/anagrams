CC=c99
BINS=server main.o builder.o accept.o client

all: $(BINS)

debug: debug_server d_main.o d_builder.o d_accept.o d_connect.o

clean:
	rm $(BINS)

server: builder.o accept.o connect.o main.o
	$(CC) -o server builder.o accept.o connect.o main.o

client: client_view.c
	$(CC) client_view.c -lpanel -lncurses -o client

main.o: main.c
	$(CC) -c main.c

builder.o: builder.c builder.h
	$(CC) -c builder.c

accept.o: accept.c
	$(CC) -c accept.c

connect.o: connect.c
	$(CC) -c connect.c

debug_server: d_builder.o d_accept.o d_connect.o d_main.o
	$(CC) -g -o debug_server builder.o accept.o connect.o main.o

d_main.o: main.c
	$(CC) -g -c main.c

d_builder.o: builder.c builder.h
	$(CC) -g -c builder.c

d_accept.o: accept.c
	$(CC) -g -c accept.c

d_connect.o: connect.c
	$(CC) -g -c connect.c
