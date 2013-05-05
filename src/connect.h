// defines the functions in connect.c for use by main

int start_server();
int get_player_index(int port);
void *get_in_addr(struct sockaddr *sa);
int get_biggest_player_fd();
