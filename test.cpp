#include <stdio.h>
#include <typeinfo>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
using namespace std;

void set_laserdata(double (&laser)[361], char (&inbuffer)[4096]);
void set_encoder(int (&Rencoder), int (&Lencoder), char (&inbuffer)[4096]);

int main(){
	// create a socket
	int sock = 0;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		printf("ERROR: Fail to create a socket!");

	char IPaddress[] = {"xxx.xxx.xx.xxx"}
	
	// connect the socket
	struct sockaddr_in info;
	bzero(&info, sizeof(info));
	info.sin_family = AF_INET;
	info.sin_addr.s_addr = inet_addr(IPaddress);
	info.sin_port = htons(1024);

	int err = connect(sock, (struct sockaddr *)&info, sizeof(info));
	if (err == -1){
		printf("Connection Error");
		return -1;
	}

	// Start TCP IP
	double laser_data[361] = {0.0};
	int Rencoder, Lencoder;

	// Send vel_cmd
	char sendmsg1[] = {"vel 1.0 0.0 0.0 0.3 0.0"};
	send(sock, sendmsg1, sizeof(sendmsg1), 0);
	sleep(1);

	char sendmsg2[] = {"vel -1.0 0.0 0.0 0.3 0.0"};
	send(sock, sendmsg2, sizeof(sendmsg2), 0);
	sleep(1.5);

	// Acquire laser data
	char sendmsg3[] = {"laser"};
	send(sock, sendmsg3, sizeof(sendmsg3), 0);

	char buffer[4096] = {};  // Used by socket.recv
	memset(buffer, 0, sizeof(buffer));
	recv(sock, buffer, sizeof(buffer), 0);
	set_laserdata(laser_data, buffer);

	memset(buffer, 0, sizeof(buffer));
	recv(sock, buffer, sizeof(buffer), 0);
	set_laserdata(laser_data, buffer);

	for (int i=0; i<361; i++)
		cout << laser_data[i] << " ";
	cout << endl;

	// Acquire encoder
	char sendmsg4[] = {"enc"};
	send(sock, sendmsg4, sizeof(sendmsg4), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(sock, buffer, sizeof(buffer), 0);
	set_encoder(Rencoder, Lencoder, buffer);
	printf("Rencoder: %d ||| Lencoder: %d \n", Rencoder, Lencoder);

	// Send stop command
	char sendmsg5[] = {"stop"};
	send(sock, sendmsg5, sizeof(sendmsg5), 0);
	
	return 0;
}
	
void set_laserdata(double (&laser)[361], char (&inbuffer)[4096]){
	char* pch;
	char spliter[] = " ";
	pch = strtok(inbuffer, spliter);

	if (strcmp(pch, "0~180") == 0){
		for (int i=0; i<180; i++){
			pch = strtok(NULL, spliter);
			laser[181+i] = atof(pch);
		}
	}
	else if (strcmp(pch, "540~720") == 0){
		for (int i=0; i<180; i++){
			pch = strtok(NULL, spliter);
			laser[1+i] = atof(pch);
		}
	}
	else
		printf("%s:%d:func:%s()  error: Invalid Laser Ranges!!\n", __FILE__, __LINE__, __func__); 
}

void set_encoder(int (&Rencoder), int (&Lencoder), char (&inbuffer)[4096]){
	char* pch;
	char spliter[] = " ";
	pch = strtok(inbuffer, spliter);

	if (strcmp(pch,"enc") == 0){
		Rencoder = atoi(strtok(NULL, spliter));
		Lencoder = atoi(strtok(NULL, spliter));
	}
	else
		printf("%s:%d:func:%s()  error:Invalid encoder name!!\n", __FILE__, __LINE__, __func__); 
}
