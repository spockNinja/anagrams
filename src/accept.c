#include "server.h"
#include "builder.h"
#include "word_list.h"

#define DEBUG 0


#if DEBUG

Player p[] = {
    { 0, 0, "bob", 8081, 1, 1},
    { 0, 1, "jill", 8082, 1, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1},
    { 0, 1, "", 8082, 0, 1}
};


struct word_node base = {"llamas", "aallms", 6, NULL};




struct word_node fours = {"mall", "allm", 4, NULL};

struct word_node fivetemp1 = {"small", "aallms", 5, NULL};
struct word_node fives = {"llama", "aallm", 5, &fivetemp1} ;

struct word_set factors = {
    NULL,
    &fours,
    &fives,
    NULL,
    NULL,
    NULL
};

int usedwords[2] = {2, 3};

Server_Info server_info = {
    p,
    "8080",
    2,
    20,
    5,
    usedwords,
    &base,
    &factors
};

#endif

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
bool valid_word(const char* test_word){

    bool is_valid = false;
    char* t_word = word_sort(test_word);
    int tw_len = strlen(t_word);
    struct word_node* list = malloc( sizeof(struct word_node) );

    switch(tw_len){
        case 3:
            list = server_info.base_word_factors->threes;
            break;
        case 4:
            list = server_info.base_word_factors->fours;
            break;
        case 5:
            list = server_info.base_word_factors->fives;
            break;
        case 6:
            list = server_info.base_word_factors->sixes;
            break;
        case 7:
            list = server_info.base_word_factors->sevens;
            break;
        case 8:
            list = server_info.base_word_factors->eights;
            break;
        default:
            list = NULL;
    }

    while(list != NULL){
        if (DEBUG) printf("checking list word: %s\n", list->word);
        int cmp = strcmp(list->word, test_word);
        if (cmp == 0) is_valid = true;
        list = list->next;
    }

    free(t_word);
    free(list);
	return is_valid;
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



#if DEBUG
/**
 * test function for this module
 */
int main(int argc, char* argv[]){
    char* b_word = server_info.base_word->word;

    char* test_1 = "llsm"; // not in b_word
    char* test_2 = "mall";  // in b_word
    char* test_3 = "small"; // in b_word

    printf("%s -> %s\n", b_word, word_sort(b_word));

    printf("%s -> %s, %s\n", test_1, word_sort(test_1),
            valid_word(test_1) ? "true" : "false");
    
    printf("%s -> %s, %s\n", test_2, word_sort(test_2),
            valid_word(test_2) ? "true" : "false");

    printf("%s -> %s, %s\n", test_3, word_sort(test_2),
            valid_word(test_3) ? "true" : "false");

}
#endif
