/* This part of the program creates the inital set 
 * of connections in the alloted time, or with a
 * server setup, and stores it in player structs
 * and in an FD_SET which gets returned.
 */

#include "server.h"

//this gets passed a Server Info struct that is in the main program 
//for general information, and passes back an fd_set for the list
//of all connected parties, hopefully to make later selects easier.
fd_set start_server(Server_Info *serverinfo)
{
	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int biggest_fd;   // largest file descriptor number
	return master;
]