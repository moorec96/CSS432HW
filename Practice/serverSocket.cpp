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

struct thread_data{
	int sd;
	int repetition;
};

void* thread_server(void* input){
	thread_data *data = (thread_data *)(input);
	char databuf[BUFSIZE];
	int count = 0; 
	for(int i = 0; i < data->repetition; i++){
		for(int nRead = 0; (nRead += read(data->sd, databuf, BUFSIZE - nRead)) < BUFSIZE; ++count);
		count++;
	}
	//printf("Count: " + count);
	write(data->sd, &count, sizeof(count));
	close(data->sd);
}

int main(int argc, char* argv[]){
	const int server_port = atoi(argv[1]);
	const int repetition = atoi(argv[2]);
	const int n = 5;

	sockaddr_in acceptSockAddr;
	bzero((char*)&acceptSockAddr, sizeof(acceptSockAddr));
	acceptSockAddr.sin_family = AF_INET;
	acceptSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	acceptSockAddr.sin_port = htons(server_port);

	int serverSd = socket(AF_INET, SOCK_STREAM, 0);
	

	const int on = 1;
	setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int));	//Make it so the socket can be resused 

	bind(serverSd, (sockaddr*)&acceptSockAddr, sizeof(acceptSockAddr));	//Bind socket to address

	listen(serverSd,n);

	sockaddr_in newSockAddr;
	socklen_t newSockAddrSize = sizeof(newSockAddr);

	while(1){
		int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
		printf("Connected to new client");
		
		pthread_t new_thread;
		struct thread_data *data = new thread_data;
		data->repetition = repetition;
		data->sd = newSd;
		pthread_create(&new_thread, NULL, thread_server, (void*)data);
	}
	close(serverSd);
}
