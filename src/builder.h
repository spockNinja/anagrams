// Defines things that external files need to know about builder.c

#define MIN_WORD_SIZE 3
#define MAX_WORD_SIZE 8

struct word_node* read_list(Server_Info* server, FILE* word_list);

void pick_word(Server_Info* server, struct word_node* list_head);

void generate_game_words(Server_Info* server, struct word_node* list_head);
