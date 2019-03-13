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
	char IPaddress[] = {"xxx.xxx.xx.xxx"};
	vector<int> ports({8001,8002,8003});

	// create sockets
	int sock_laser = 0;
	sock_laser = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_laser == -1)
		printf("ERROR: Fail to create a socket!");	
	
	// connect the socket
	struct sockaddr_in info_laser;
	bzero(&info_laser, sizeof(info_laser));
	info_laser.sin_family = AF_INET;
	info_laser.sin_addr.s_addr = inet_addr(IPaddress);
	info_laser.sin_port = htons(ports[0]);

	int err_laser = connect(sock_laser, (struct sockaddr *)&info_laser, sizeof(info_laser));
	if (err_laser == -1){
		printf("Connection Error\n");
		return -1;
	}

	// create sockets
	int sock_enc = 0;
	sock_enc = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_enc == -1)
		printf("ERROR: Fail to create a socket!");	
	
	// connect the socket
	struct sockaddr_in info_enc;
	bzero(&info_enc, sizeof(info_enc));
	info_enc.sin_family = AF_INET;
	info_enc.sin_addr.s_addr = inet_addr(IPaddress);
	info_enc.sin_port = htons(ports[1]);

	int err_enc = connect(sock_enc, (struct sockaddr *)&info_enc, sizeof(info_enc));
	if (err_enc == -1){
		printf("Connection Error\n");
		return -1;
	}

	// create sockets
	int sock_vel = 0;
	sock_vel = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_vel == -1)
		printf("ERROR: Fail to create a socket!");	
	
	// connect the socket
	struct sockaddr_in info_vel;
	bzero(&info_vel, sizeof(info_vel));
	info_vel.sin_family = AF_INET;
	info_vel.sin_addr.s_addr = inet_addr(IPaddress);
	info_vel.sin_port = htons(ports[2]);

	int err_vel = connect(sock_vel, (struct sockaddr *)&info_vel, sizeof(info_vel));
	if (err_vel == -1){
		printf("Connection Error\n");
		return -1;
	}

	// Start TCP IP
	double laser_data[361] = {0.0};
	int Rencoder, Lencoder;

	// Send vel_cmd
	char sendmsg1[] = {"vel 1.0 0.0 0.0 0.3 0.0"};
	send(sock_vel, sendmsg1, sizeof(sendmsg1), 0);
	sleep(1);

	char sendmsg2[] = {"vel -1.0 0.0 0.0 0.3 0.0"};
	send(sock_vel, sendmsg2, sizeof(sendmsg2), 0);
	sleep(1.5);

	// Acquire laser data
	char sendmsg3[] = {"laser"};
	send(sock_laser, sendmsg3, sizeof(sendmsg3), 0);

	char buffer[4096] = {};  // Used by socket.recv
	memset(buffer, 0, sizeof(buffer));
	recv(sock_laser, buffer, sizeof(buffer), 0);
	set_laserdata(laser_data, buffer);

	memset(buffer, 0, sizeof(buffer));
	recv(sock_laser, buffer, sizeof(buffer), 0);
	set_laserdata(laser_data, buffer);

	for (int i=0; i<361; i++)
		cout << laser_data[i] << " ";
	cout << endl;

	// Acquire encoder
	char sendmsg4[] = {"enc"};
	send(sock_enc, sendmsg4, sizeof(sendmsg4), 0);
	memset(buffer, 0, sizeof(buffer));
	recv(sock_enc, buffer, sizeof(buffer), 0);
	set_encoder(Rencoder, Lencoder, buffer);
	printf("Rencoder: %d ||| Lencoder: %d \n", Rencoder, Lencoder);

	// Send stop command
	char sendmsg5[] = {"stop"};
	send(sock_vel, sendmsg5, sizeof(sendmsg5), 0);
	
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
