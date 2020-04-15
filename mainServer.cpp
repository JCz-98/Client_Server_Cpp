//
//  main.cpp
//  Server_C++
//
//  Created by Jonathan Cazco on 3/17/20.
//  Copyright © 2020 Jonathan Cazco. All rights reserved.
//

#include <iostream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

static void usage();

int main(int argc, char *argv[])
{
    if (argc > 1 && *(argv[1]) == '-')
    {
        usage(); exit(1);
    }
    
    int listenPort = 1234;
    if (argc > 1)
        listenPort = atoi(argv[1]);
    
    // Create a socket
    int s0 = socket(AF_INET, SOCK_STREAM, 0);
    if (s0 < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    // Fill in the address structure containing self address
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(listenPort);        // Port to listen
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // Bind a socket to the address
    int res = bind(s0, (struct sockaddr*) &myaddr, sizeof(myaddr));
    if (res < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    // Set the "LINGER" timeout to zero, to close the listen socket
    // immediately at program termination.
    struct linger linger_opt = { 1, 0 }; // Linger active, timeout 0
    setsockopt(s0, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
    

    
    //i Now, listen for a connection
    res = listen(s0, 25);    // "1" is the maximal length of the queue
    if (res < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    std::cout << "Esperando cliente..." << std::endl;
    
    // Accept a connection (the "accept" command waits for a connection with
    // no timeout limit...)
    struct sockaddr_in peeraddr;
    socklen_t peeraddr_len;
    int s1 = accept(s0, (struct sockaddr*) &peeraddr, &peeraddr_len);
    if (s1 < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    
    // A connection is accepted. The new socket "s1" is created
    // for data input/output. The peeraddr structure is filled in with
    // the address of connected entity, print it.
    std::cout << "Connection from IP "
    << ( ( ntohl(peeraddr.sin_addr.s_addr) >> 24) & 0xff ) << "."  // High byte of address
    << ( ( ntohl(peeraddr.sin_addr.s_addr) >> 16) & 0xff ) << "."
    << ( ( ntohl(peeraddr.sin_addr.s_addr) >> 8) & 0xff )  << "."
    <<   ( ntohl(peeraddr.sin_addr.s_addr) & 0xff ) << ", port "   // Low byte of address
    << ntohs(peeraddr.sin_port);
    
    std::cout << "\n";

    res = close(s0);    // Close the listen socket
    
    //  write(s1, "Hello!\r\n", 8);
    
    char buffer[1024];
    bool connection = true;
    char ok[] = "0003OK\0";
    char error[] = "0005ERROR\0";

    pid_t pid;

    int pfd[2];
/*
    if (pipe(pfd) == -1)
	    fprintf(stderr, "Error creating pipe\n");

    switch(pid = fork()){
    	
	    case -1:
		    fprintf(stderr, "Error creating new child process\n");
		    exit(EXIT_FAILURE);
	    case 0:
		    //pipe for stdout
		    if (close(pfd[1])==-1)
			    fprintf(stderr,"close pipe end writed\n");
		    if (pfd[0] != STDIN_FILENO){
			if (dup2(pfd[0], STDIN_FILENO) == -1)
				fprintf(stderr, "dup2 end read pipe\n");
			if (close(pfd[0]) == -1)
				fprintf(stderr, "close end pipe read");
		    }
			char *cmd[4];
			cmd[0]="./client";
			cmd[1]="186.101.153.136";
			cmd[2]="9090";
			cmd[3] = NULL;

		    execv("./client", cmd);
		    fprintf(stderr,"Error exec de client\n");
		
	    default:
		    break;
		   

    }
    if(pfd[1] != STDOUT_FILENO){
    	if (dup2(pfd[1], STDOUT_FILENO) == -1)
		fprintf(stderr,"error write pipe\n");
	if (close(pfd[1])==-1)
		fprintf(stderr,"error closing parent end write");
    }
	
    if (close(pfd[0]) == -1)
	    fprintf(stderr, "closing parent read pipe\n");
*/
    while(connection == true)
    {
        res = read(s1, buffer, 1023);
        if (res < 0) 
	{
            std::cerr << "\nError: " << strerror(errno) << std::endl;
            //char response[] = "0005Error";
	    send(s1, error, strlen(error), 0);
	   //kill(pid, SIGKILL);
	    close(s1);
	    exit(1);
        }
        if(res == 0)
        {
           std::cerr << "\nWarning: Connection closed" << std::endl;
	   //kill(pid, SIGKILL);
            close(s1);
            return 0;
        }
        else{
		buffer[res]=0;
		std::cout << "\nReceived " << res << " bytes:\n" << buffer;

		if (strcmp(buffer, "0011DISCONNECT\0") == 0){
			
			send(s1, ok, strlen(ok), 0);
			//kill(pid, SIGKILL);
			close(s1);
			return 0;
		}
			
		buffer[res]=0;
            //buffer[res] = '\n';
	    //buffer[res+1] = 0;
          // std::cout << "\nReceived " << res << " bytes:\n" << buffer;
	 // std::cout << buffer;
	    //fprintf(stderr, "\n");
            //std::cout << "\nConfirmation sent to client! " << "\n\n";
            send(s1, ok, strlen(ok), 0);
	  
        }
    }

    return 0;
    
}

static void usage() {
    std::cout << "A simple Internet server application.\n"
    << "It listens to the port written in command line (default 1234),\n"
    << "accepts a connection, and sends the \"Hello!\" message to a client.\n"
    << "Then it receives the answer from a client and terminates.\n\n"
    << "Usage:\n"
    << "     server [port_to_listen]\n"
    << "Default is the port 1234.\n";
}
