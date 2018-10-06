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

int main(int argc, char* argv[]){
	const int server_port = atoi(argv[1]);
	//printf(server_port);
	const int repetition = atoi(argv[2]);
	const int nbufs = atoi(argv[3]);
	const int bufsize = atoi(argv[4]);
	char *server_name = argv[5];
	const int type = atoi(argv[6]);

	char databuf[nbufs][bufsize];

	struct timeval start, end, lap;

	struct hostent* host = gethostbyname(server_name);
	
	//This sets up data structure that contains all the needed socket info
	sockaddr_in sendSockAddr;										//Create a new socket variable
	bzero((char*) &sendSockAddr, sizeof(sendSockAddr));	//zero out that memory location
	sendSockAddr.sin_family = AF_INET;							//Set the socket to IPv4
	sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	sendSockAddr.sin_port = htons(server_port);				//Set the port to the servers port
	

	//Creates a socket that is stream-oriented and apart of the Internet Address family
	int clientSd = socket(AF_INET, SOCK_STREAM, 0);

	int connectStatus = connect(clientSd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
	if(connectStatus < 0){
		printf("Failed to connect to server");
	}
	
	gettimeofday(&start,NULL);
	for(int i = 0; i < repetition; i++){
		switch(type){
		case 1: 
			for(int j = 0; j < nbufs; j++){
				int x = write(clientSd, databuf[j], bufsize);
				if(x < 0){
					printf("Problem with write");
				}
			}	
			break;
		case 2:
			{
				struct iovec vector[nbufs];
				for(int j = 0; j < nbufs; j++){
					vector[j].iov_base = databuf[j];
					vector[j].iov_len = bufsize;
				}
				int x = writev(clientSd,vector,nbufs);
				if(x < 0){
					printf("Problem with write");
				}
			}
			break;
		case 3:
			{	
				int x = write(clientSd,databuf, nbufs * bufsize);
				if(x < 0){
					printf("Problem with write");
				}
			}	
			break;
		default:
			break;
		}
	}
	
	int count = 0; 
	read(clientSd, &count, sizeof(count));
	gettimeofday(&end,NULL);
	printf("Test: Data-Sending Time = %ld usec, Round-Trip Time = %ld usec, #Reads = %d\n",(end.tv_usec - start.tv_usec)/repetition, end.tv_usec - 					start.tv_usec,count);
	close(clientSd);
}
