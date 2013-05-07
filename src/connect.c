/* This part of the program creates the inital set 
 * of connections in the alloted time, or with a
 * server setup, and stores it in player structs
 * and in an FD_SET which gets returned.
 */

#include "server.h"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int get_player_index(int port)
{
    for(int i = 0; i<=server_info.num_players;i++)
    {
        if(server_info.players[i].portnumber == port)
            return i;
    }
    return -1;
}

int get_biggest_player_fd()
{
    int biggest = 0;
    for(int i = 0; i<=server_info.num_players;i++)
    {
        biggest = server_info.players[i].portnumber > biggest ? server_info.players[i].portnumber : biggest;
    }
    return biggest;
}

char *check_name(char* test_name)
{
    char* temp_name = calloc(strlen(test_name)+1, sizeof(char));
    strcpy(temp_name, test_name);
    int num_same = 0;
    for(int i = 0; i<=server_info.num_players;i++)
    {
        if(server_info.players[i].username != null_player.username && 
           strcmp(server_info.players[i].username, temp_name) == 0)
        {
            num_same++;
            printf("above asprintf\n");
            asprintf(&temp_name, "%s%d", test_name, num_same);
            printf("checking new name: %s\n", temp_name);
            i = 0;
        }
        printf("finished with index %d\n", i);
    }
    return temp_name;    
}

//this gets passed a Server Info struct that is in the main program 
//for general information, and passes back an fd_set for tnhe list
//of all connected parties, hopefully to make later selects easier.
int start_server()
{
    fd_set read_fds;  // temp file descriptor list for select()
    int biggest_fd;   // largest file descriptor number 

	int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&server_info.current_users);    // clear the master and temp sets
    FD_ZERO(&read_fds);


    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    char *ext_port = server_info.port; 
    
    if ((rv = getaddrinfo(NULL, ext_port, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    if (listen(listener, 10) == -1) {
    // listen
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &server_info.current_users);
    
    server_info.listen_fd = listener;
    // keep track of the biggest file descriptor
    biggest_fd = listener; // so far, it's this one
	int select_result = 0;
	struct timeval tv;
	tv.tv_sec = 30;
	tv.tv_usec = 0;
	pthread_t timethread;
    struct targ time_arg;
    time_arg.t = &tv;
    time_arg.interval = 1;
    
	if(pthread_create(&timethread, NULL,(void*) &timer, &time_arg) != 0)
	{
	    perror("cannot create thread");
	}
	
	for(;;) {
        read_fds = server_info.current_users; // copy it
		printf("the time starting left is %i seconds.\n", (int)tv.tv_sec );
		select_result = select(biggest_fd+1, &read_fds, NULL, NULL, &tv);
        if (select_result == -1) {
            perror("select");
            exit(4);
        }
		else if(select_result)
		{
			printf("the time left is %i seconds.\n", (int)tv.tv_sec );
        	// run through the existing connections looking for data to read
	        for(i = 0; i <= biggest_fd; i++) {
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
	                        
	                        if(server_info.num_players+1 <= max_users){
	                            server_info.num_players++;
	                            Player new_player;
	                            new_player.portnumber = newfd;
	                            new_player.connected = true;
	                            new_player.points = 0;
	                            new_player.bonus_points = 0;
	                            new_player.username[0] = '\0';
	                            for(int j = 1; j<=max_users;j++)
	                            {
	                                if(server_info.players[j].portnumber == null_player.portnumber)
	                                {
	                                    new_player.color = j;
	                                    server_info.players[j] = new_player;
	                                    break;
	                                }       
	                            }
	                            FD_SET(newfd, &server_info.current_users); // add to master set
	                            if (newfd > biggest_fd) {    // keep track of the max
	                                biggest_fd = newfd;
	                            }
	                        }
	                        else
	                        {
	                            printf("connection on socket %d refused\n", newfd);
	                            //write to the port that they're too late   
	                            char *tempstr = "You were too late! There are already ten people connected.\nGoodbye!\n";
	                            write(newfd, tempstr, strlen(tempstr)+1);
	                            close(newfd);
	                        }
	                            
	                    }
	                } else {
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
	                	    //current player index
	                	    int cpi = get_player_index(i);
	                	    buf[nbytes] = '\0';
	                	    //parse client message
	                	    char code;
	                	    char* message = calloc(nbytes*2, sizeof(char));
	                	    sscanf(buf, "%c%[^;]", &code, message);
                            if(code == 'n')
                            {
                                message = check_name(message);
                                strncpy(server_info.players[cpi].username, message, nbytes);
                                //server_info.players[cpi].username[USERNAME_LEN] = '\0'; // strncpy doesn't do this
                                printf("User on fd(%d) at index %d changed their username to: %s\n", i, cpi, message);
                                message = update_name(cpi, message);
                                write(i, message, strlen(message)+1);
                            }
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
