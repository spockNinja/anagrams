#include "server.h"
#include "builder.h"
#include "connect.h"


// The main server method
// takes the port as the first (mandatory) argument
// and an optional number of rounds
int main (int argc, char* argv[]) {

    if (argc == 1) {
        fprintf(stderr, "Please specify the port number as the first argument.\n");
        exit(EXIT_FAILURE);
    }

    Server_Info server_info;
    fd_set current_users;

    server_info.port = argv[1];

    if (argc == 3) {
        server_info.num_rounds = strtol(argv[2], NULL, 10);
        if (server_info.num_rounds == 0) {
            fprintf(stderr, "The specified number of rounds is not valid.\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        server_info.num_rounds = def_rounds;
    }

    // read in the word list
    char* word_dict = "../res/words";
    char ORIG_LIST[MAX_WORDS][MAX_SIZE];
    char SORT_LIST[MAX_WORDS][MAX_SIZE];

    // Open the file for reading
    server_info.word_list = fopen(word_dict, "r");
    if (server_info.word_list == NULL) {
        perror(word_dict);
        exit(EXIT_FAILURE);
    }

    read_list(&server_info, ORIG_LIST, SORT_LIST);

    // initialise server field memory
    server_info.used_words = calloc(server_info.num_rounds, sizeof(int));
    memset(server_info.used_words, 0, server_info.num_rounds);

    server_info.base_word = calloc(MAX_SIZE, sizeof(char));
    server_info.base_word_sorted = calloc(MAX_SIZE, sizeof(char));
    server_info.base_word_factors = calloc(MAX_WORDS, sizeof(char*));

    FD_ZERO(&current_users);
    start_server(&server_info, &current_users);

    // once players are connected, play through all the rounds
    int round;
    srand(time(0) + getpid());
    for (round=0; round < server_info.num_rounds; round++) {
        pick_word(&server_info, ORIG_LIST, SORT_LIST);
        generate_game_words(&server_info, ORIG_LIST, SORT_LIST);
    }
}
