#include "server.h"
#include "builder.h"

#define DEBUG 0

// character compare function
// A helper function used by qsort that sorts each word
// see qsort documentation if you're curious
int cmp_char(const void* word1, const void* word2) {
    char a = *((char*)word1);
    char b = *((char*)word2);
    if (a > b) return 1;
    if (a < b) return -1;
    return 0;
}

// word character sorting function
// accepts a word as a parameter
// returns the word with its characters sorted
char* word_sort(const char* word){
    char* word_s = strdup(word);
    qsort(word_s, strlen(word), sizeof(char), cmp_char);
    return word_s;
}


/**
* checks if the given word is valid for this round
*/
bool valid_word(const char* base_word, const char* test_word){
	// FIXME: needs to be implemented.
    char* b_word = word_sort(base_word);
    char* t_word = word_sort(test_word);
    if (strlen(t_word) > strlen(b_word)) return false;

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

#if DEBUG
/**
 * test function for this module
 */
int main(int argc, char* argv[]){
    char* b_word = "llamas";

    char* test_1 = "pie"; // not in b_word
    char* test_2 = "small"; // in b_word

    printf("%s -> %s\n", b_word, word_sort(b_word));

    printf("%s -> %s, %s\n", test_1, word_sort(test_1),
        valid_word(b_word, test_1) ? "true" : "false");

    printf("%s -> %s, %s\n", test_2, word_sort(test_2),
            valid_word(b_word, test_2) ? "true" : "false");

}
#endif
