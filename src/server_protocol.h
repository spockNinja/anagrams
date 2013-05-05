void message_clients(const char* command);

char* update_time(int seconds);

char* update_name(unsigned int player_index, char* player_name);

char* update_score(unsigned int player_index, unsigned int score);

char* update_bword(char* bword);

char* update_skeleton(struct word_set* base_word_factors);
