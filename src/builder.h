
#define MIN_SIZE 3
#define MAX_SIZE 9 // 8 chars + '\0'
#define MAX_WORDS 40000

int sort_word(const void* word1, const void* word2);

void read_list(Server_Info* server, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]);

void pick_word(Server_Info* server, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]);

void generate_game_words(Server_Info* server, char orig_list[MAX_WORDS][MAX_SIZE], char sort_list[MAX_WORDS][MAX_SIZE]);
