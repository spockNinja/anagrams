void message_clients(const char* command);

char* update_time(int seconds);

char* update_round_number(int num);

char* update_slot(int slot_index, int player_index, char* word);

char* update_name(unsigned int player_index, char* player_name);

char* update_score(unsigned int player_index, unsigned int score);

char* update_slot(int slot_index, int player_index, char* word);

char* update_bword(char* bword);

char* update_skeleton(struct word_set* base_word_factors);
