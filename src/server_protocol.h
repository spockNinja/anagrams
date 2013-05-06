void message_clients(const char* command);

char* update_time(int seconds);

char* update_name(int player_index, char* player_name);

char* update_player_list();

char* update_leaderboard();

char* update_round_number(int num);

char* update_slot(int slot_index, int player_index, char* word);

char* update_bword(char* bword);

char* update_skeleton(struct word_set* base_word_factors);
