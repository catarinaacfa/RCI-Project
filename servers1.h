#ifndef SERVERS1_H
#define SERVERS1_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>


#include "auxi1.h"

#define SIZE_MSG 141 /* Maximum msg size */

#define ID 0
#define MSG 1

typedef struct parameters
{
	int numberMessages; /* Number of messages the server wants to know */
	char message[SIZE_MSG]; /* Message to publish */
	struct _t_lista *messages_to_publish; /* Holds the messages that rmb wants to publish */

}Parameters;

typedef struct udp
{
	char *serv_ip;
	int serv_port;
	int socket_udp;
	struct sockaddr_in serveraddr;
	int flag_available; /* Checks if message server is available to communicate */
	int flag_check_publish;
	int flag_check_show_msg;
	struct timeval start_time_check;
	char buffer[28000];

}UDP_Socket;

void establishedUDPsocket(UDP_Socket *udp, int serverUDP);
void sendToServer(UDP_Socket *udp, char *msg);
void receiveFromServer(UDP_Socket *udp);
void receiveFromServer(UDP_Socket *udp);
void closeSockets(UDP_Socket *udp_msgserver, UDP_Socket *udp_id);
void freeAllMemory(UDP_Socket *udp_id, Parameters *params);
void closeAndFree(UDP_Socket *udp_msgserver, UDP_Socket *udp_id, Parameters *params);

#endif
