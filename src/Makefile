CC=c99
BINS=server client
OBJS=main.o builder.o accept.o word_list.o
D_BINS=debug_server debug_client d_main.o d_builder.o d_accept.o d_connect.o d_word_list.o
SERVER_BINS=main.o builder.o connect.o accept.o word_list.o server_protocol.o
D_SERVER_BINS=d_main.o d_builder.o d_accept.o d_connect.o d_word_list.o

all: $(BINS) $(OBJS)

debug: $(D_BINS)

clean:
	rm $(OBJS)
	rm $(BINS)

d_clean:
	rm $(D_BINS)

server: $(SERVER_BINS)
	$(CC) $(SERVER_BINS) -o server

client: client.c word_list.o
	$(CC) client.c -lpanel -lncurses word_list.o -o client

word_list.o: word_list.c word_list.h
	$(CC) -c word_list.c

main.o: main.c
	$(CC) -c main.c

builder.o: builder.c builder.h
	$(CC) -c builder.c

accept.o: accept.c
	$(CC) -c accept.c

connect.o: connect.c
	$(CC) -c connect.c
	
server_protocol.o: server_protocol.c
	$(CC) -c server_protocol.c

debug_server: d_builder.o d_accept.o d_connect.o d_word_list.o d_main.o
	$(CC) -g -o debug_server d_builder.o d_accept.o d_connect.o d_word_list.o d_main.o

d_main.o: main.c
	$(CC) -g -c main.c

d_builder.o: builder.c builder.h
	$(CC) -g -c builder.c

d_accept.o: accept.c
	$(CC) -g -c accept.c

d_connect.o: connect.c
	$(CC) -g -c connect.c

d_word_list.o: word_list.c word_list.h
	$(CC) -g -c word_list.c
