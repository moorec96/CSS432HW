/**
	Program: Makes HTTP GET Requests to a web server and saves the returned data
	Author: Caleb Moore
	Date: 10/20/18
**/

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
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;


//This method receives a string that contains file data and runs through it looking for 
//the keyword "src" which is in the tags of images and scripts. If the those tags are found
//Their links are placed into a vector and returned
vector<string> findTags(string& buffer){
	vector<string> res;
	vector<int> indexes;
	int index = 0;
	while(index != string::npos && index < buffer.length()){
		index = buffer.find("SRC",index+1);
		if(index != string::npos){
			indexes.push_back(index);
		}
	} 
	
	for(int i = 0; i < indexes.size(); i++){
		int idx = indexes[i]+5;
		string temp = "";
		while(buffer[idx] != '\"'){
			temp += buffer[idx++];
		}
		
		int index = temp.length()-1;
		while(temp[index] > 0 && !isprint(temp[index])){
			index--;
		}
		string newFileName = "";
		for(int i = 0; i < index+1; i++){
			newFileName += temp[i];
		}
		newFileName[index+1] = '\0';
		res.push_back(newFileName);
	}	
	return res;
}


//This method receives a file name, hostname, and port number and opens a socket to communicate with the server
//It then will make a GET request with the parameters given and then fill a string with the received data from the server
string makeRequest(char* fileName, char* hostName, char* portNumber){
	struct hostent* host = gethostbyname(hostName);
	
	int retrieverSd = socket(AF_INET,SOCK_STREAM, 0);
	if(retrieverSd < 0){
		printf("Error: Could not create socket\n");
		close(retrieverSd);
		exit(0);
	}

	sockaddr_in sendSockAddr;
	bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
	sendSockAddr.sin_family = AF_INET;
	sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	sendSockAddr.sin_port = htons(atoi(portNumber));

	int connectStatus = connect(retrieverSd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
	if(connectStatus < 0){
		printf("Failed connect to the server");
	}


	char req[] = "GET ";
	strcat(req,fileName);
	strcat(req," http/1.1\r\nHost: ");
	strcat(req,hostName);
	strcat(req,"\r\nAccept: text/html, image/png, image/jpeg, image/gif");
	strcat(req,"\r\n\r\n");
	int len = 0;
	char resp[512];
	string response = "";
	send(retrieverSd, req, strlen(req),0);
	cout<< req << endl;
	while((len = recv(retrieverSd, resp,512,0)) > 0){
		resp[len+1] = '\0';
		response.append(resp);
	}
	
	close(retrieverSd);
	return response;
}


//This method receives the buffer that is returned from the makeRequest() function and saves that data into a 
//file
void placeIntoFile(string& buffer, string fileName){
	int index = buffer.find("\r\n\r\n");
	cout << buffer.substr(0,index+1) << endl << endl<< endl << endl << endl << endl;
	string temp = buffer.substr(index+4);
	char fName[fileName.length()];
	strcpy(fName,fileName.c_str());
	ofstream file;
	file.open(fName);
	file << temp;
	file.close();
}


//This method returns only a files name, and not its entire path
string getFileName(char* file){
	string res = "";
	int index = strlen(file)-1;
	while(index > 0 && file[index]!= '/'){
		res = file[ index--] + res;
	}

	return res;
}

//Main runs the first request, and then depending on if there are other images/scripts in the file returned,
//it will call more requests on those files as well
int main(int argc, char* argv[]){
	char* portNumber = argv[3];
	string response = makeRequest(argv[2],argv[1],portNumber);
	string fileName = getFileName(argv[2]);
	placeIntoFile(response, fileName);
	if(fileName.find("html") != string::npos){
		vector<string> tags = findTags(response);
		for(int i = 0; i < tags.size(); i++){
			char fName[tags[i].length()+5];
			fName[0] = '/';
			fName[1] = 'e';
			fName[2] = 'v';
			fName[3] = 'i';
			fName[4] = 'l';
			fName[5] = '/';
			for(int j = 0; j < tags[i].length(); j++){
				fName[j+6] = tags[i][j];
			}
			fName[tags[i].length() + 6] = '\0';
			string f = makeRequest(fName,argv[1],portNumber);
			fileName = getFileName(fName);
			placeIntoFile(f,fileName);
		}
	}
	

	//close(retrieverSd);
}
