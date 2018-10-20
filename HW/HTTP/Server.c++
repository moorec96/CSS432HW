/**
	Purpose: Create a multi-threaded server that a client can connect to
	Author: Caleb Moore
	Date: 10/06/2018
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
using namespace std;

const int BUFSIZE = 1500;

int main(int argc, char* argv[]){
	const int server_port = 8080;
	int n = 5;

	//Set up socket data structure 
	sockaddr_in acceptSockAddr;
	bzero((char*)&acceptSockAddr, sizeof(acceptSockAddr));
	acceptSockAddr.sin_family = AF_INET;
	acceptSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	acceptSockAddr.sin_port = htons(server_port);

	int serverSd = socket(AF_INET, SOCK_STREAM, 0);
	
	//Make it so the socket can be reused
	const int on = 1;
	setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int));	//Make it so the socket can be resused 

	bind(serverSd, (sockaddr*)&acceptSockAddr, sizeof(acceptSockAddr));	//Bind socket to address
	listen(serverSd,n);
	sockaddr_in newSockAddr;
	socklen_t newSockAddrSize = sizeof(newSockAddr);
	
	long valread;
	while(1){	//Wait for a socket to make a request 
		int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
		printf("Connected to new client\n");
		
		char buffer[512] = {0};
		valread = read(newSd,buffer,512);
		cout << buffer << endl;
		char msg[] = "You reached the server.";
		write(newSd, msg, strlen(msg));
		close(newSd);
	}
	close(serverSd);	//Close socket
}
