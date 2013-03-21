// This program will take the words found in /460/words
// pick a starting word (7, 8, 9 chars long?)
// and use that word to make all possible words

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int MIN_SIZE = 3;
int MAX_SIZE = 8;
int MAX_WORDS = 40000;

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
void read_list(FILE* word_file, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]) {

    char current_word[30];
    int list_loc = 0;

    while(!feof(word_file)) {
        fgets(current_word, 30, word_file);
        int word_size = strlen(current_word);

        // strip out newlines
        if (current_word[word_size-1] == '\n') {
            current_word[word_size-1] = '\0';
            word_size = word_size-1;
        }
        // only include the word if it is the right size
        if (word_size >= MIN_SIZE && word_size <= MAX_SIZE) {
            strcpy(orig_list[list_loc], current_word);
            qsort(current_word, word_size, 1, sort_word);
            strcpy(sort_list[list_loc], current_word);
            printf("%s %s\n", orig_list[list_loc], sort_list[list_loc]);
        }
    }
}

void generate_game_words(char* base_word, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]) {

}

// Usually runs with no args, but can take an optional word list location
// * Not needed when we get to compiling the server - the server will just call read_list
//      after setting up the same things*
int main (int argc, char* argv[]) {
    char* SRC_LIST = "/460/words";
    char ORIG_LIST[MAX_WORDS][MAX_SIZE];
    char SORT_LIST[MAX_WORDS][MAX_SIZE];

    if (argc == 2) {
        SRC_LIST = argv[1];
    }

    // Open the file for reading
    FILE* word_file = fopen(SRC_LIST, "r");

    if (word_file == NULL) {
        perror(SRC_LIST);
        exit(EXIT_FAILURE);
    }

    read_list(word_file, ORIG_LIST, SORT_LIST);
}
