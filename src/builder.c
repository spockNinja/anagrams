// This file has all the methods to handle puzzle generation
// including reading the word list, picking a base word, and generating puzzle words

#include "server.h"
#include "builder.h"

// A helper function used by qsort that sorts each word
// see qsort documentation if you're curious
int sort_word(const void* word1, const void* word2) {
    char a = *((char*)word1);
    char b = *((char*)word2);
    if (a > b) return 1;
    if (a < b) return -1;
    return 0;
}

// Reads every word in a file (each word on a line)
// and stores the normal list in orig_list while storing a parallel list of sorted words in sort_list
void read_list(Server_Info* server, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]) {

    char current_word[30];
    int list_loc = 0;

    while(!feof(server->word_list)) {
        fgets(current_word, 30, server->word_list);
        int word_size = strlen(current_word);

        // strip out newlines
        if (current_word[word_size-1] == '\n') {
            current_word[word_size-1] = '\0';
            word_size = word_size-1;
        }
        // only include the word if it is the right size
        if (word_size >= MIN_SIZE && word_size < MAX_SIZE) {
            strcpy(orig_list[list_loc], current_word);
            qsort(current_word, word_size, 1, sort_word);
            strcpy(sort_list[list_loc], current_word);
            list_loc++;
        }
    }

    printf("Word Count: %d", list_loc);
    server->total_words = list_loc;
}

// picks a random word from the word list (length of MAX_SIZE) to use as the base_word
void pick_word(Server_Info* server, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]) {
    bool found = false;

    while(!found) {
        //int i = rand() % MAX_WORDS;
        int i = rand() % server->total_words;
        for (; i <= MAX_WORDS; i++) {
            if (strlen(orig_list[i]) == MAX_SIZE-1) {
                int j;
                bool used = false;
                for(j=0; j < server->num_rounds; j++) {
                    if (server->used_words[j] == i) {
                        used = true;
                        break;
                    }
                    else if (server->used_words[j] == 0) {
                        break;
                    }
                }

                if (!used) {
                    strcpy(server->base_word, orig_list[i]);
                    strcpy(server->base_word_sorted, sort_list[i]);
                    server->used_words[j] = i;
                    return;
                }
            }
        }
    }
}

// generates a list of words based on the chosen base_word
void generate_game_words(Server_Info* server, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]) {
    int comp_index = 0;
    int list_loc = 0;

    while (sort_list[comp_index] != NULL && comp_index < MAX_WORDS) {
        int base_letter = 0;
        int comp_letter = 0;
        // check each letter
        while (sort_list[comp_index][comp_letter] != '\0' && server->base_word_sorted[base_letter] != '\0') {
            // if they are equal, continue
            if (sort_list[comp_index][comp_letter] == server->base_word_sorted[base_letter]) {
                comp_letter++;
                base_letter++;
            }
            // if root letter is lower, increment
            else if(server->base_word_sorted[base_letter] < sort_list[comp_index][comp_letter]) {
                base_letter++;
            }
            // not a match, break out
            else {
                break;
            }
        }

        // add the word if it fits
        if (sort_list[comp_index][comp_letter] == '\0' && strlen(sort_list[comp_index]) != 0) {
            server->base_word_factors[list_loc] = orig_list[comp_index];
            printf("\tBase word %d: %s\n", list_loc, server->base_word_factors[list_loc]);
            list_loc++;
        }

        comp_index++;
    }
}
