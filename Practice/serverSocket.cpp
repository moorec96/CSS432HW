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


const int BUFSIZE = 1500;

//Struct that contains socket data
struct thread_data{
	int sd;
	int repetition;
};

//Function that is called when a new thread is created
void* thread_server(void* input){
	struct timeval start, end;	//Used for keeping track of time

	thread_data *data = (thread_data *)(input);	//Initialize struct that contains client socket data
	char databuf[BUFSIZE];								
	int count = 0; 
	gettimeofday(&start,NULL);							//Start timer
	for(int i = 0; i < data->repetition; i++){	//Loop through and read from client data 
		for(int nRead = 0; (nRead += read(data->sd, databuf, BUFSIZE - nRead)) < BUFSIZE; ++count);
		count++;
	}
	gettimeofday(&end,NULL);							//End timer
	write(data->sd, &count, sizeof(count));		//Write count back to client

	long seconds = end.tv_sec - start.tv_sec;		//This code prevents the time output from being negative
	long microSec = end.tv_usec - start.tv_usec;
	if(microSec < 0){
		seconds -= 1;
	}
	long totalTime = (seconds * 1000000) + abs(microSec);

	printf("\nData-Receiving Time:%ld usec\n", totalTime);
	close(data->sd);	//Close Socket
}

int main(int argc, char* argv[]){
	const int server_port = atoi(argv[1]);		//Take in arguments and set port and repetition
	const int repetition = atoi(argv[2]);
	const int n = 5;

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

	while(1){	//Wait for a socket to make a request 
		int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
		printf("Connected to new client");
		

		pthread_t new_thread;									//create a new thread to handle socket request 
		struct thread_data *data = new thread_data;
		data->repetition = repetition;
		data->sd = newSd;
		pthread_create(&new_thread, NULL, thread_server, (void*)data);	//Have new thread run thread_server function
	}
	close(serverSd);	//Close socket
}
