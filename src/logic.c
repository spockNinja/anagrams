/* This handles most of the game logic
 * and most of the situations that might
 * come up within the setting of one round
 */
#include "server.h"

int start_game()
{
    fd_set read_fds;  // temp file descriptor list for select()
    int biggest_fd = get_biggest_player_fd();   // largest file descriptor number 
    int select_result = 0;
    int newfd;        // newly accept()ed socket descriptor
    printf("the biggest fd is: %d\n", biggest_fd);
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

	struct timeval tv;
	tv.tv_sec = 30;
	tv.tv_usec = 0;
	int listener = server_info.listen_fd;
	
	for(;;) {
        
        read_fds = server_info.current_users; // copy it
		select_result = select(biggest_fd+1, &read_fds, NULL, NULL, NULL);
        if (select_result == -1) {
            perror("select");
            exit(4);
        }
		else if(select_result)
		{
			printf("the time left is %i seconds.\n", (int)tv.tv_sec );
			
        	// run through the existing connections looking for data to read
	        for(int i = 0; i <= biggest_fd; i++) {
	            if (FD_ISSET(i, &read_fds)) { // we got one!!
	                if (i == listener) {
	                    // handle new connections
	                    addrlen = sizeof remoteaddr;
	                    newfd = accept(listener,
	                        (struct sockaddr *)&remoteaddr,
	                        &addrlen);
						
	                    if (newfd == -1) {
	                        perror("accept");
	                    } else {
	                        printf("selectserver: new connection from %s on "
	                            "socket %d\n", inet_ntop(remoteaddr.ss_family,
	                                           get_in_addr((struct sockaddr*)&remoteaddr),
	                                           remoteIP, INET6_ADDRSTRLEN),
    	                                       newfd);
	                        
                            printf("connection on socket %d refused\n", newfd);
                            //write to the port that they're too late   
                            char *tempstr = "You were too late! The game is in progress.\nGoodbye!\n";
                            write(newfd, tempstr, strlen(tempstr)+1);
                            close(newfd);   
	                        
	                            
	                    }
	                } else {
	                    int nbytes;
	                    // handle data from a client
	                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
	                        // got error or connection closed by client
	                        if (nbytes == 0) {
	                            // connection closed
	                            printf("selectserver: socket %d hung up\n", i);
	                            server_info.players[get_player_index(i)].connected = false;
	                            server_info.num_players--;
	                            }
	                        else {
	                            perror("recv");
	                        }
	                        close(i); // bye!
	                        FD_CLR(i, &server_info.current_users); // remove from master set
	                	}
	                	//they didn't disconnect
	                	else
	                	{
	                	    //current player index
	                	    int cpi = get_player_index(i);
	                	    
	                	    //parse client message
	                	    char code;
	                	    char* message;
	                	    sscanf(buf, "%c%s;", &code, message);
                            if(code == 'w')
                            {
                                printf("%s sent the word: %s\n", server_info.players[cpi].username, message);
                                if(valid_word(message))
                                {
                                    server_info.players[cpi].points += word_value(message);
                                    server_info.players[cpi].bonus_points += word_bonus(message);
                                }
                                else
                                    write(i, "&;", 3);
                            }
                            
                            message = update_score(cpi, (server_info.players[cpi].points + server_info.players[cpi].bonus_points));
                            message_clients(message);
                            
	                	} 
	                } // END handle data from client
	            } // END got new incoming connection
	        } // END looping through file descriptors
		} // END else if for if this is a thing
		else
		{
		    // the select timed out
			break;
		}
    } // END for(;;)--and you thought it would never end!

	printf("Done looking for users (30 seconds elapsed)\n");
	
    return 0;
}
