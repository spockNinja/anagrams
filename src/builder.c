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
struct word_node* read_list(FILE* word_list) {

    struct word_node* previous_node = NULL;
    struct word_node* current_node = NULL;
    struct word_node* head_node = NULL;
    char current_word[30];

    while(!feof(word_list)) {
        char* read_result = fgets(current_word, 30, word_list);
        if (read_result == NULL) {
            break;
        }
        int word_size = strlen(current_word);

        // strip out newlines
        if (current_word[word_size-1] == '\n') {
            current_word[word_size-1] = '\0';
            word_size = word_size-1;
        }
        // only include the word if it is the right size
        if (word_size >= MIN_WORD_SIZE && word_size <= MAX_WORD_SIZE) {
            // copy and sort the word
            char current_word_sorted[30];
            strcpy(current_word_sorted, current_word);
            qsort(current_word_sorted, word_size, sizeof(char), sort_word);

            // create the new node
            current_node = create_node(current_word, current_word_sorted, word_size);

            // remember the head node to send back to main
            if (head_node == NULL) {
                head_node = current_node;
            }
            // keep the list linked even though we are using malloc
            if (previous_node != NULL) {
                previous_node->next = current_node;
            }

            // move to the next node
            previous_node = current_node;
            current_node = current_node->next;
            server_info.total_words++;
        }
    }

    return head_node;
}

// picks a random word from the word list (length of MAX_WORD_SIZE) to use as the base_word
void pick_word(struct word_node* list_head) {
    bool found = false;
    struct word_node* current_node = list_head;

    while(!found) {
        // pick a random spot in the list to start
        int i = rand() % server_info.total_words;

        // get to that part of the list
        int j;
        for(j=0; j<i && current_node->next != NULL;j++) {
            current_node = current_node->next;
        }

        // pick the closest valid word
        while(current_node != NULL) {
            if (current_node->len == MAX_WORD_SIZE) {
                bool used = false;
                struct word_node* next_word = server_info.used_words;
                while (next_word != NULL) {
                    if (strcmp(next_word->word, current_node->word) == 0) {
                        used = true;
                        break;
                    }
                    next_word = next_word->next;
                }

                if (!used) {
                    // Use create_node so that no chains are broken
                    server_info.base_word = create_node(current_node->word, current_node->sorted_word, current_node->len);
                    next_word = create_node(current_node->word, current_node->sorted_word, current_node->len);
                    return;
                }
            }
            current_node = current_node->next;
        }
    }
}

// generates a list of words based on the chosen base_word
void generate_game_words(struct word_node* list_head) {
    int words_found = 0;
    struct word_node* current_node = list_head;

    while (current_node != NULL) {
        int base_letter = 0;
        int comp_letter = 0;
        // check each letter
        while (current_node->sorted_word[comp_letter] != '\0' && server_info.base_word->sorted_word[base_letter] != '\0') {
            // if they are equal, continue
            if (current_node->sorted_word[comp_letter] == server_info.base_word->sorted_word[base_letter]) {
                comp_letter++;
                base_letter++;
            }
            // if root letter is lower, increment
            else if(server_info.base_word->sorted_word[base_letter] < current_node->sorted_word[comp_letter]) {
                base_letter++;
            }
            // not a match, break out
            else {
                break;
            }
        }

        // add the word if it fits
        if (current_node->sorted_word[comp_letter] == '\0') {
            struct word_node* add_to_list;
            switch(current_node->len) {
                case 3:
                    add_to_list = server_info.base_word_factors->threes;
                    break;
                case 4:
                    add_to_list = server_info.base_word_factors->fours;
                    break;
                case 5:
                    add_to_list = server_info.base_word_factors->fives;
                    break;
                case 6:
                    add_to_list = server_info.base_word_factors->sixes;
                    break;
                case 7:
                    add_to_list = server_info.base_word_factors->sevens;
                    break;
                case 8:
                    add_to_list = server_info.base_word_factors->eights;
                    break;
            }

            // get to the end of the list
            while(add_to_list != NULL) {
                add_to_list = add_to_list->next;
            }

            // copy the node *can't use the same pointer because it will mess up the source list*
            add_to_list = create_node(current_node->word, current_node->sorted_word, current_node->len);
            words_found++;

        }

        current_node = current_node->next;
    }
}

char get_rare_char(const char* base){
    
    //least common character
    int base_len = strlen(base);
    int lcc = base_len;
    char result;
    for(int i = 0; i<base_len;)
    {
        //current character count
        int ccc = 0;
        //base character
        char bc[2] = {base[i], '\0'};
        for(i; i<base_len;i++)
        {
            if (base[i] == bc[0])
                ccc++;
            else
                break;
        }
        //result = (ccc < lcc ? &bc : ccc == lcc ? strcat(result, &bc):result);
        if(ccc<=lcc)
        {
            lcc = ccc;
            result = bc[0];
        }
    }
    return result;
}
