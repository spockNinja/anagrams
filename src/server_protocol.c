#include "server_protocol.h"

#define SERVER_PROTOCOL_DEBUG 0

void message_clients(const char* command){
#if SERVER_PROTOCOL_DEBUG
    write( STDOUT_FILENO, command, strlen(command));
    return;
#endif

    int fdmax = 0;
    for (int i = 0; i < server_info.num_players; i++){
        if(FD_ISSET(i, server_info.current_users)) {
            if (send(i, command, strlen(command)+1, 0) == -1)
                fprintf(stderr, "message failure to fd#%i, message: %s\n", i, command);
        }
    }

}

char* update_time(char* time_string){
    char* cmd;
    asprintf(&cmd, "t%s;\n", time_string); 
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


#if SERVER_PROTOCOL_DEBUG
int main(int argc, char* argv[]){

    message_clients(update_time("5:00"));

    message_clients(update_score(1, 9000));

    message_clients(update_name(3, "Billy"));

}
#endif
