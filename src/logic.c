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
	                        FD_SET(newfd, &server_info.current_users); // add to master set
	                        if (newfd > biggest_fd) {    // keep track of the max
	                            biggest_fd = newfd;
	                        }
	                        printf("selectserver: new connection from %s on "
	                            "socket %d\n", inet_ntop(remoteaddr.ss_family,
	                                           get_in_addr((struct sockaddr*)&remoteaddr),
	                                           remoteIP, INET6_ADDRSTRLEN),
    	                                       newfd);
	                        
	                        if(server_info.num_players+1 <= max_users){
	                            server_info.num_players++;
	                            Player new_player;
	                            new_player.portnumber = newfd;
                                    printf("stupid port... %i\n", newfd);
	                            new_player.connected = true;
	                            new_player.points = 0;
	                            for(int j = 1; j<=max_users;j++)
	                            {
	                                if(server_info.players[j].portnumber == null_player.portnumber)
	                                {
	                                    new_player.color = j;
	                                    server_info.players[j] = new_player;
	                                    break;
	                                }       
	                            }
	                        }
	                        else
	                        {
	                            printf("connection on socket %d refused\n", newfd);
	                            //write to the port that they're too late   
	                        }
	                            
	                    }
	                } else {
	                    int nbytes;
	                    // handle data from a client
	                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
	                        // got error or connection closed by client
	                        if (nbytes == 0) {
	                            // connection closed
	                            printf("selectserver: socket %d hung up\n", i);
	                            server_info.players[get_player_index(i)] = null_player;
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
	                	    char tempbuf[nbytes];
                            if(buf[0] == 'n')
                            {
                                for(int j=1; j<nbytes; j++)
                                {
                                    tempbuf[j-1] = buf[j];
                                }
                                tempbuf[nbytes] = '\0';
                                printf("port %i is now named: %s\n", i, tempbuf);
                            }
                            printf("they pressed a button\n");
                            write(i, "&\n" ,3);
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
