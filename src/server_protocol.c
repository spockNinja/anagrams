#include "server.h"

#define SERVER_PROTOCOL_DEBUG 0
 
void message_clients(const char* command){
#if SERVER_PROTOCOL_DEBUG
    write( STDOUT_FILENO, command, strlen(command));
    return;
#endif

    for (int i = 0; i <= server_info.num_players; i++){
        if(FD_ISSET(server_info.players[i].portnumber, &(server_info.current_users))) {
            if (write(server_info.players[i].portnumber, command, strlen(command)+1) == -1)
                fprintf(stderr, "message failure to fd#%i, message: %s\n", i, command);
        }
    }

}

char* update_time(int seconds){
    char* cmd;
    asprintf(&cmd, "t%i;\n", seconds); 
    return cmd;
}

char* update_name(int player_index, char* player_name){
    char* cmd;
    asprintf(&cmd, "n%u,%s;\n", player_index, player_name);
    return cmd;
}

int compar_player(const void* player1, const void* player2){
    Player* p1 = (Player*) player1;
    Player* p2 = (Player*) player2;
    return (p1->points+p1->bonus_points) < (p2->points+p2->bonus_points) ? 1 : (p1->points+p1->bonus_points) == (p2->points+p2->bonus_points) ? 0 : -1;
}

char* player_data(int player_index){
    Player* p = &server_info.players[player_index];
    char* cmd;
    char* empty_string = "";
    if(p->portnumber == null_player.portnumber) return empty_string;

    asprintf(&cmd, "%i:%u:%s,", p->color, p->points+p->bonus_points, p->username);
    return cmd;
}

char* update_player_list(){
    char* cmd; 

    // printf("preparing to sort the players\n");
    qsort(&server_info.players[1], server_info.num_players, sizeof (Player), compar_player);
    // printf("sorted the players\n");
    
    printf("building player list string\n");

    asprintf(&cmd, "p%s%s%s%s%s%s%s%s%s%s;\n", 
           player_data(1),
           player_data(2),
           player_data(3),
           player_data(4),
           player_data(5),
           player_data(6),
           player_data(7),
           player_data(8),
           player_data(9),
           player_data(10)
           );
    return cmd;
}

char* end_player_data(int player_index){
    Player* p = &server_info.players[player_index];
    char* cmd;
    char* empty_string = "";
    if(p->portnumber == null_player.portnumber) return empty_string;
    char* username = calloc(strlen(p->username)+6, sizeof(char));
    username = p->username;
    if(p->connected == false)
    {
        int i = strlen(p->username);
        username[i+1] = '(';
        username[i+2] = 'D';
        username[i+3] = 'N';
        username[i+4] = 'F';
        username[i+5] = ')';
    }
    asprintf(&cmd, "%i:%u-%u:%s,", p->color, p->points, p->bonus_points, username);
    return cmd;
}

char* update_leaderboard(){
    char* cmd; 

    // printf("preparing to sort the players\n");
    qsort(&server_info.players[1], server_info.num_players, sizeof (Player), compar_player);
    // printf("sorted the players\n");
    
    printf("building player list string\n");

    asprintf(&cmd, "*%s%s%s%s%s%s%s%s%s%s;\n", 
           end_player_data(1),
           end_player_data(2),
           end_player_data(3),
           end_player_data(4),
           end_player_data(5),
           end_player_data(6),
           end_player_data(7),
           end_player_data(8),
           end_player_data(9),
           end_player_data(10)
           );
    return cmd;
}

char* update_round_number(int num){
    char* cmd;
    asprintf(&cmd, "r%i;\n", num);
    return cmd;
}

char* update_slot(int slot_index, int player_index, char* word){
    char* cmd;
    int word_length = strlen(word);
    char* raw_cmd = calloc(2*word_length+1, sizeof(char*));
    char lccs = server_info.rare_char;
    int i, j;
    for (i = 0, j = 0; i < word_length; i++){
        if (word[i] == lccs){
            raw_cmd[j] = '.';
            j = j+1;
        }
        raw_cmd[j] = word[i];
        j = j+1;
    }
    if (word_length == max_base_word_length){
        raw_cmd[j] = '*';
    }

    asprintf(&cmd, "w%i-%i,%i%s;\n", word_length, slot_index, player_index, raw_cmd);
    free(raw_cmd);
    return cmd;
}

char* update_bword(char* bword){
    char* cmd;
    int word_length = strlen(bword);
    char* raw_cmd = calloc(2*word_length+1, sizeof(char*));
    char lccs = server_info.rare_char;
    bool found_lccs = false;
    int i, j;
    for (i = 0, j = 0; i < word_length; i++){
        if (!found_lccs && bword[i] == lccs){
            raw_cmd[j] = '.';
            j = j+1;
            found_lccs = true;
        }
        raw_cmd[j] = bword[i];
        j = j+1;
    }

    asprintf(&cmd, "b%s;\n", raw_cmd);
    free(raw_cmd);
    return cmd;
}

char* update_skeleton(struct word_set* base_word_factors){
    char* cmd;
    asprintf(&cmd, "l3:%i,4:%i,5:%i,6:%i,7:%i,8:%i;\n",
            count_words(base_word_factors->threes),
            count_words(base_word_factors->fours),
            count_words(base_word_factors->fives),
            count_words(base_word_factors->sixes),
            count_words(base_word_factors->sevens),
            count_words(base_word_factors->eights));
    return cmd;
}

#if SERVER_PROTOCOL_DEBUG
int main(int argc, char* argv[]){

    message_clients(update_time(300));

    message_clients(update_score(1, 9000));

    message_clients(update_name(3, "Billy"));

    struct timeval* countdown;
    countdown->tv_sec = 300; // 5 minutes
    countdown->tv_usec = 0;
    
    timer(countdown, 1);

}
#endif
