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
#include <fstream>
using namespace std;

const int BUFSIZE = 1500;
const char *http_response_200 = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
const char *http_response_404 = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><h1>ERROR 404</h1><i>Not Found!</i></body></html>";



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

char* getFile(char* buffer){
	string fileName = getFileName(buffer);
	cout << fileName << endl;
	//FILE *file;
	string buff;
	ifstream file(fileName.c_str());
	if(file.fail()){
		cout << "Could not find file" << endl;
		buff = http_response_404;
	}
	else{
		//cout << "Found File" << endl;
		buff = http_response_200;
		//fclose(file);
		string line;
		getline(file,line);
		while(!file.eof()){
			getline(file,line);
			buff += line;
		}
		file.close();
	}
/*
	if(file = fopen(fileName.c_str(),"r")){
		cout << "Found File" << endl;
		buff = http_response_200;
		fclose(file);
	}	
	else{
		cout << "Could not find file" << endl;
		buff = http_response_404;
	}*/

	char* res = new char[buff.length()+1];
	strcpy(res,buff.c_str());
	return res;
}

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
		char* fileBuff = getFile(buffer);
		send(newSd, fileBuff, strlen(fileBuff),0);
		//send(newSd, http_response_404,strlen(http_response_404),0);
		close(newSd);
	}
	close(serverSd);	//Close socket
}
