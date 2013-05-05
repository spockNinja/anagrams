#include "server.h"

#define SERVER_PROTOCOL_DEBUG 0
 
void message_clients(const char* command){
#if SERVER_PROTOCOL_DEBUG
    write( STDOUT_FILENO, command, strlen(command));
    return;
#endif

    for (int i = 0; i <= server_info.num_players; i++){
        if(FD_ISSET(server_info.players[i].portnumber, &(server_info.current_users))) {
            if (send(server_info.players[i].portnumber, command, strlen(command)+1, 0) == -1)
                fprintf(stderr, "message failure to fd#%i, message: %s\n", i, command);
        }
    }

}

char* update_time(int seconds){
    char* cmd;
    asprintf(&cmd, "t%i;\n", seconds); 
    return cmd;
}

char* update_name(unsigned int player_index, char* player_name){
    char* cmd;
    asprintf(&cmd, "n%u%s;\n", player_index, player_name);
    return cmd;
}

char* update_score(unsigned int player_index, unsigned int score){
    char* cmd;
    asprintf(&cmd, "s%u%u;\n", player_index, score);
    return cmd;
}

char* update_bword(char* bword){
    char* cmd;
    // FIXME: include a '.' character in front of the bonus character
    asprintf(&cmd, "b%s;\n", bword);
    return cmd;
}

char* update_skeleton(struct word_set* base_word_factors){
    char* cmd;
    asprintf(&cmd, "l3:%i,4:%i,5:%i,6:%i,7:%i,8:%i;",
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
