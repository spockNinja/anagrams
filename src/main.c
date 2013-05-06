#include "server.h"
#include "builder.h"
#include "connect.h"
#include "logic.h"

// Resets server words for each round
void reset_words(Server_Info* server) {
    free_list(server->base_word_factors->threes);
    free_list(server->base_word_factors->fours);
    free_list(server->base_word_factors->fives);
    free_list(server->base_word_factors->sixes);
    free_list(server->base_word_factors->sevens);
    free_list(server->base_word_factors->eights);
}

// The main server method
// takes the port as the first (mandatory) argument
// and an optional number of rounds
int main (int argc, char* argv[]) {

    if (argc == 1) {
        fprintf(stderr, "Please specify the port number as the first argument.\n");
        exit(EXIT_FAILURE);
    }

    server_info.port = argv[1];

    if (argc >= 3) {
        server_info.num_rounds = strtol(argv[2], NULL, 10);
        if (server_info.num_rounds == 0) {
            fprintf(stderr, "The specified number of rounds is not valid.\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        server_info.num_rounds = def_rounds;
    }

    // if a different word list is to be used
    // a 3rd arg should be passed in
    char* word_list_path;
    if (argc == 4) {
        word_list_path = argv[3];
    }
    else {
        word_list_path = "/460/words";
    }
    // Open the file for reading
    FILE* word_list = fopen(word_list_path, "r");
    if (word_list == NULL) {
        perror(word_list_path);
        exit(EXIT_FAILURE);
    }    
    
    // initialize null player
    null_player.points = 0;
    null_player.bonus_points = 0;
    null_player.color = 0;
    null_player.username = "";
    null_player.portnumber = 0;
    null_player.connected = false;
    null_player.ready = false;
    
    // initialise server variables
    server_info.num_players = 0;
    server_info.total_words = 0;
    for(int i = 0; i<=max_users; i++)
        server_info.players[i] = null_player;
    server_info.used_words = NULL;
    server_info.base_word_factors = malloc(sizeof(struct word_set));
    server_info.base_word_factors->threes = NULL;
    server_info.base_word_factors->fours = NULL;
    server_info.base_word_factors->fives = NULL;
    server_info.base_word_factors->sixes = NULL;
    server_info.base_word_factors->sevens = NULL;
    server_info.base_word_factors->eights = NULL;

    struct word_node* list_head = read_list(word_list);

    FD_ZERO(&(server_info.current_users));

    start_server();

    // once players are connected, play through all the rounds
    int round;
    srand(time(0) + getpid());
    for (round=0; round < server_info.num_rounds; round++) {
        reset_words(&server_info);
        pick_word(list_head);
        generate_game_words(list_head);
        server_info.rare_char = get_rare_char(server_info.base_word->sorted_word);
        printf("The word for round %d is: %s\n", round, server_info.base_word->word);
        printf("With a rare character of: %s\n", server_info.rare_char);
       start_game();
    }
}

