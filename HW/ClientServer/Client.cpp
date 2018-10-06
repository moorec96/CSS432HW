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

using namespace std;

int main(int argc, char* argv[]){
	struct hostent* host = gethostbyname(argv[0]);

	int port = 10695;
	sockaddr_in sendSockAddr;
	bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
	sendSockAddr.sin_family = AF_INET;
	sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	sendSockAddr.sin_port = htons(atoi(argv[1]));

	int clientSd = socket(AF_INET,SOCK_STREAM, 0);

	int connectStatus = connect(clientSd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
	if(connectStatus < 0){
		printf("Failed to connect to the server");
	}

	for(int i = 0; i < repetition; i++){
		for(int j = 0; j < nbufs; j++){
			int x = write(clientSd, databuf[j], bufsize);
			if(x < 0){
				printf("Problem with write");
			}
		}
	}
	
	int count = 0;
	read(clientSd, &count, sizeof(count));
	close(clientSd);
}
