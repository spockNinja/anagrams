#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct player {
	int points;
	int color;		// 0-9 preset colors
	char* username;
	int portnumber;	// do we need this?
	bool connected;	// whether or not the client is currently connected
	bool ready;		// whether or not the client is ready for a round
}

struct serverinfo {
	struct player* players;
	int num_players;
	char* base_word;			// the root word
	char* base_word_sorted;		// base_word with letters in alphabetic order
	char* base_word_factors;	// the list of words able to be made from base_word, in alphabetic order
}