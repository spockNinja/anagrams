#include "server_protocol.h"

#define SERVER_PROTOCOL_DEBUG 1

void message_clients(const char* command){
#if SERVER_PROTOCOL_DEBUG
    write( STDOUT_FILENO, command, strlen(command));
    return;
#endif


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
