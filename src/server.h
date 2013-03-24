#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ext_port "9876"  //external port we listen on
#define max_users 10

typedef struct{
	int points;		// current tally of the player's points
	int color;		// 0-9 preset colors
	char* username; // hurr durr, this is the username
	int portnumber;	// do we need this? #MAYBE
	bool connected;	// whether or not the client is currently connected
	bool ready;		// whether or not the client is ready for a round
} Player;

typedef struct {
	Player* players;		
	int num_players;			// to compare with the server maximum
	char* base_word;			// the root word
	char* base_word_sorted;		// base_word with letters in alphabetic order
	char* base_word_factors;	// the list of words able to be made from base_word, in alphabetic order
} Server_Info;