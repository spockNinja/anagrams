// This file encapsulates all logic for manipulating linked word lists

#include "server.h"

// A helper function to create and initialise a new word_node
struct word_node* create_node(char* word, char* sorted_word, int len) {
    struct word_node* new_node = (struct word_node*) malloc(sizeof(struct word_node));
    new_node->word = malloc(sizeof(char) * (len+1));
    new_node->sorted_word = malloc(sizeof(char) * (len+1));

    strcpy(new_node->word, word);
    strcpy(new_node->sorted_word, sorted_word);
    new_node->len = len;
    new_node->next = NULL;

    return new_node;
}

// Frees all the memory for a list that starts with head
void free_list(struct word_node* head) {
    while(head != NULL) {
        struct word_node* temp = head;
        head = head->next;
        free(temp);
    }
}

int count_words(struct word_node* head){
    int count = 0;
    while (head != NULL){
        count = count+1;
        head = head->next;
    }
    return count;
}
