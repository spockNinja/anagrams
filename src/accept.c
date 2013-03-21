#include <server.h>

/**
* checks if the given word is valid for this round
*/
bool valid_word(char* word){
	// FIXME: needs to be implemented.
	return true;
}


/**
* Returns the number of points the given word is worth.
* Assumes that the given word is valid.
*/
int word_value(char* word){

	int word_length = 0;
	int score = 0;

	switch(word_length){
		case 2:
			score = 2;
			break;
		case 3:
			score = 3;
			break;
		case 4:
			score = 5;
			break;
		case 5:
			score = 8;
			break;
		case 6:
			score = 12;
			break;
		case 7:
			score = 17;
			break;
		case 8:
			score = 22;
			break;
		case 9:
			score = 28;
			break;
		case 10:
			score = 35;
			break;
		case 11:
			score = 43;
			break;
		case 12:
			score = 52;
			break;
		case 13:
			score = 95;
			break;
	}
	return score;
}


/**
* adds the given score to the current player p's score
*/
void update_score(Player* p, int score){
	p->points += score;
	// FIXME: do what's needed to update the client
}