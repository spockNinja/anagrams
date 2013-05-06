// Defines things that external files need to know about builder.c

#define MIN_WORD_SIZE 3
#define MAX_WORD_SIZE 8

struct word_node* read_list(FILE* word_list);

void pick_word(struct word_node* list_head);

void generate_game_words(struct word_node* list_head);

char get_rare_char(const char* base);
