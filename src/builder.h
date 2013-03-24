
#define MIN_SIZE 3
#define MAX_SIZE 8
#define MAX_WORDS 40000

int sort_word(const void* word1, const void* word2);

void read_list(FILE* word_file, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]);

void generate_game_words(char* base_word, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]);
