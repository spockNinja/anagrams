#include "server.h"
#include "builder.h"

#define DEBUG 0

#if DEBUG
#include "test.h"
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
int valid_word(const char* test_word){

    char* t_word = word_sort(test_word);
    int tw_len = strlen(t_word);
    struct word_node* list = malloc( sizeof(struct word_node) );
    struct word_node* used_list = malloc( sizeof(struct word_node) );

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

    used_list = server_info.used_word_factors;
    while (used_list != NULL){
        int cmp = strcmp(used_list->word, test_word);
        if (cmp == 0) return -1;
        used_list = used_list->next;
    }

    int place = 0;
    while(list != NULL){
        if (DEBUG) printf("checking list word: %s\n", list->word);
        int cmp = strcmp(list->word, test_word);
        if (cmp == 0){
            // add it to used_word_factors
            struct word_node* used_factor = create_node(test_word,(char*) word_sort(test_word), (int)strlen(test_word));
            used_factor->next = server_info.used_word_factors;
            server_info.used_word_factors = used_factor;
            return place;
        }
        list = list->next;
        place++;
    }


    free(t_word);
    free(list);
	return -1;
}


/**
* Returns the number of base points the given word is worth.
* Assumes that the given word is valid.
*/
unsigned int word_value(char* word){

	int word_length = strlen(word);
	unsigned int score = 0;

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
* Returns the number of bonus points the given word is worth.
* Assumes that the given word is valid.
*/
unsigned int word_bonus(char* word){

    unsigned int score = 0;
    int word_length = strlen(word);
    char lccs = server_info.rare_char;
    for (int i = 0; i < word_length; i++){
       if (word[i] == lccs) score += 5;        
    }

    if (word_length == server_info.base_word->len) score += 100;

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
