#include "server.h"
#include "builder.h"


// Usually runs with no args, but can take an optional word list location
// * Not needed when we get to compiling the server - the server will just call read_list
//      after setting up the same things*
int main (int argc, char* argv[]) {
    
	Server_Info server_info;
	fd_set current_users;
	
	char* SRC_LIST = "../res/words";
    char ORIG_LIST[MAX_WORDS][MAX_SIZE];
    char SORT_LIST[MAX_WORDS][MAX_SIZE];
	
    if (argc == 2) {
        SRC_LIST = argv[1];
    }

    // Open the file for reading
    FILE* word_file = fopen(SRC_LIST, "r");

    if (word_file == NULL) {
        perror(SRC_LIST);
        exit(EXIT_FAILURE);
    }

    read_list(word_file, ORIG_LIST, SORT_LIST);
	
	FD_ZERO(&current_users);
	start_server(&server_info, &current_users);
}
