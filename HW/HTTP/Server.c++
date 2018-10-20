/**
	Purpose: Create a simplified web server that takes in HTTP GET requests and returns data 
	Author: Caleb Moore
	Date: 10/20/2018
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
#include <fstream>
using namespace std;

const int BUFSIZE = 1500;
const char *http_response_200 = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
const char *http_response_404 = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><h1>ERROR 404</h1><i>Not Found!</i></body></html>";


//This method finds the file that is being asked for in the GET response
string getFileName(char* buffer){
	string getReq = string(buffer);
	int index;
	if(index = getReq.find("GET") == string::npos){
		printf("ERROR: Invalid syntax");
		exit(0);
	}

	index+=4;
	if(getReq[4] == '/') index++;
	string fileName = "";
	while(index < getReq.length() && getReq[index] != ' '){
		fileName += getReq[index++];
	}
	
	return fileName;
}

//This method checks if the file requested exists
//If the file exists then it is placed into a buffer that also contains http_response_200 and returned
//Else the http_response_404 is returned
char* getFile(char* buffer){
	string fileName = getFileName(buffer);
	string buff;
	ifstream file(fileName.c_str());
	if(file.fail()){
		buff = http_response_404;
	}
	else{
		buff = http_response_200;
		string line;
		getline(file,line);
		while(!file.eof()){
			getline(file,line);
			buff += line;
		}
		file.close();
	}
	char* res = new char[buff.length()+1];
	strcpy(res,buff.c_str());
	return res;
}


//Main opens a socket that listens for clients, and then handles their http requests
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
		
		char* fileBuff = getFile(buffer);
		cout << buffer << endl;
		send(newSd, fileBuff, strlen(fileBuff),0);
		close(newSd);
	}
	close(serverSd);	//Close socket
}
