#include "server_protocol.h"

#define SERVER_PROTOCOL_DEBUG 1

void send_update(const char* command){
    write( OUTPUT_FD, command, strlen(command));
}

void update_time(char* time_string){
    char* cmd;
    asprintf(&cmd, "t%s;\n", time_string); 
    send_update(cmd);
}

void update_name(unsigned int player_index, char* player_name){
    char* cmd;
    asprintf(&cmd, "n%u%s;\n", player_index, player_name);
    send_update(cmd);
}

void update_score(unsigned int player_index, unsigned int score){
    char* cmd;
    asprintf(&cmd, "s%u%u;\n", player_index, score);
    send_update(cmd);
}


#if SERVER_PROTOCOL_DEBUG
int main(int argc, char* argv[]){

    update_time("5:00");

    update_score(1, 9000);

    update_name(3, "Billy");

}
#endif
