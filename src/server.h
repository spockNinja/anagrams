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
#include <netdb.h>
#include <time.h>

//external port we listen on
#define ext_port "9876"

//max number of users playing the game.
#define max_users 10

// default number of rounds to be played
#define def_rounds 5


typedef struct {
	int points;		// current tally of the player's points
	int color;		// 0-9 preset colors
	char* username; // hurr durr, this is the username
	int portnumber;	// do we need this? #MAYBE
	bool connected;	// whether or not the client is currently connected
	bool ready;		// whether or not the client is ready for a round
} Player;

typedef struct {
	Player players[max_users];	// an array of all current players in the game
	char* port;                 // the communications port used for client-server comm.
	int num_players;			// to compare with the server maximum
	int num_rounds;             // number of rounds to be played
	int total_words;            // total number of words which can be made from base_word
	int* used_words;            // ?? number of words guessed ??
	struct word_node* base_word;        // the word from which all others can be made
	struct word_set* base_word_factors; // the words which can be made from base_word
} Server_Info;

struct word_node {
    char* word;
    char* sorted_word;
    int len;
    struct word_node* next;
};

struct word_set {
    struct word_node* threes;
    struct word_node* fours;
    struct word_node* fives;
    struct word_node* sixes;
    struct word_node* sevens;
    struct word_node* eights;
};

