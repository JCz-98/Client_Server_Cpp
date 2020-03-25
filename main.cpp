//
//  main.cpp
//  Cliet_Server
//
//  Created by Jonathan Cazco on 3/11/20.
//  Adapted from https://www.technical-recipes.com/2014/getting-started-with-client-server-applications-in-c/
//  Author: Andy
//  Copyright © 2020 Jonathan Cazco. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>

static void usage();

using namespace std;

int main(int argc, char *argv[])
{
    if (argc > 1 && *(argv[1]) == '-')
    {
        usage(); exit(1);
    }
    
    // Create socket
    int s0 = socket(AF_INET, SOCK_STREAM, 0);
    if (s0 < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    // Fill in server IP address
    struct sockaddr_in server; //server struct
    int serverAddrLen;
    bzero( &server, sizeof( server ) );
    
    
    char* peerHost = "localhost";
    if (argc > 1)
        peerHost = argv[1];
    
    // Resolve server address (convert from symbolic name to IP number)
    struct hostent *host = gethostbyname(peerHost);
    if (host == NULL)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    server.sin_family = AF_INET;
    short peerPort = 15151;
    if (argc >= 3)
        peerPort = (short) atoi(argv[2]);
    server.sin_port = htons(peerPort);
    
    // Print a resolved address of server (the first IP of the host)
    std::cout << "Server address = " << (host->h_addr_list[0][0] & 0xff) << "." <<
    (host->h_addr_list[0][1] & 0xff) << "." <<
    (host->h_addr_list[0][2] & 0xff) << "." <<
    (host->h_addr_list[0][3] & 0xff) << ", port " <<
    static_cast<int>(peerPort) << std::endl;
    
    // Write resolved IP address of a server to the address structure
    memmove(&(server.sin_addr.s_addr), host->h_addr_list[0], 4);
    
    // Connect to the remote server
    int res = connect(s0, (struct sockaddr*) &server, sizeof(server));
    if (res < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    bool conection = true;
    
    std::cout << "Connected!\n";
    
    char *buffer = NULL;

    size_t len = 2014;
    while (conection == true) {
        string str;
        cout << "Send message?(y/n): ";
        getline(cin,str);
        
        if(str == "y" or str == "Y")
        {
           string min;
	   cout<< "Message to server -> ";
	   getline(cin,min);
	   min += "\n";
	   char *message = const_cast<char*>(min.c_str());
            //char* message = "104.197.125.87|136877\n";
	   // message[strlen(message)]='\n';
	    //message[strlen(message)+1]= '\0';
            send(s0, message,strlen(message), 0);
            cout << endl;
	
            buffer = new char[1024];
	    
	    printf("Waiting a response:\n");
            res = read(s0, buffer, 1024);
            
	    if (res < 0)
            {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                exit(1);
            }
            std::cout << "Received from server: " << buffer << "\n\n" ;
	 
	    if (buffer != NULL){
		    delete(buffer);
	    }
	    
        }
	else if(str == "n" or str == "N")
        {
            cout << "Closing connection...\nBye\n";
            conection = false;
            close(s0);
            return 0;
        }
    }
    if (buffer != NULL)
	    delete(buffer);
    
  
     
    //send(s0, "Thanks! Bye-bye...\r\n", 20);
     
    //close(s0);
    return 0;
}

static void usage()
{
    std::cout << "A simple Internet client application.\n"
    << "Usage:\n"
    << "         client [IP_address_of_server [port_of_server]]\n"
    << "     where IP_address_of_server is either IP number of server\n"
    << "     or a symbolic Internet name, default is \"localhost\";\n"
    << "     port_of_server is a port number, default is 1234.\n"
    << "The client connects to a server which address is given in a\n"
    << "command line, receives a message from a server, sends the message\n"
    << "\"Thanks! Bye-bye...\", and terminates.\n";
}
