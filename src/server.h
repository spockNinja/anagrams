#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

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
#include "colors.h"
#include "server_protocol.h"


//external port we listen on
//#define ext_port "9876"

//max number of users playing the game.
#define max_users 10

// default number of rounds to be played
#define def_rounds 5

//Player datatype
typedef struct {
	int points;		// current tally of the player's points
	int color;		// 0-9 preset colors, also works as their index
	char* username; // hurr durr, this is the username
	int portnumber;	// the port over whhich the user is communicating
	bool connected;	// whether or not the client is currently connected
	bool ready;		// whether or not the client is ready for a round (may get to)
} Player;

//Server_Info datatype
typedef struct {
	Player players[max_users+1];	// an array of all current players in the game
	char* port;                 // the communications port used for client-server comm.
	int num_players;			// to compare with the server maximum
	int num_rounds;             // number of rounds to be played
	int total_words;                    // total number of words in the main word list
	struct word_node* used_words;       // linked list of all words tried in this game
	struct word_node* base_word;        // the word from which all others can be made *changes each round*
	struct word_set* base_word_factors; // the words which can be made from base_word *changes each round*
    char* rare_chars;      // used for bonus
    fd_set* current_users;  // all user file descriptors
} Server_Info;

// A set of linked lists, one for each possible word length
struct word_set {
    struct word_node* threes;
    struct word_node* fours;
    struct word_node* fives;
    struct word_node* sixes;
    struct word_node* sevens;
    struct word_node* eights;
};




Server_Info server_info; // global access for this instance
Player null_player;
