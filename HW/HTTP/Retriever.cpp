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
	
		//cout << newFileName << endl;
		res.push_back(newFileName);
		//cout << res[res.size()-1] << endl;
	}	
	return res;
}

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
	//send(retrieverSd, req, sizeof(req),0);
	while((len = recv(retrieverSd, resp,512,0)) > 0){
		//cout << len << endl;
		resp[len+1] = '\0';
		response.append(resp);
	}
	
	close(retrieverSd);
	return response;
}

void placeIntoFile(string& buffer, string fileName){
	int index = buffer.find("\r\n\r\n");
	cout << buffer.substr(0,index+1) << endl << endl<< endl << endl << endl << endl;
	string temp = buffer.substr(index+4);
	char fName[fileName.length()];
	strcpy(fName,fileName.c_str());
	//strcat(fName,".txt");
	ofstream file;
	file.open(fName);
	file << temp;
	file.close();
}

string getFileName(char* file){
	string res = "";
	int index = strlen(file)-1;
	while(index > 0 && file[index]!= '/'){
		res = file[ index--] + res;
	}
	
	//cout << res << endl;
	return res;
}

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
			//cout << fName << endl;
			//cout << strlen(fName) << endl;
			string f = makeRequest(fName,argv[1],portNumber);
			//cout << fName << endl;
			fileName = getFileName(fName);
			placeIntoFile(f,fileName);
		}
	}
	

	//close(retrieverSd);
}
