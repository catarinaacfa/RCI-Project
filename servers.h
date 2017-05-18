#ifndef SERVERS_H
#define SERVERS_H

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
#include <fcntl.h>
#include <signal.h>

#include "auxi.h"

#define SERVER 0
#define CLIENT 1

#define SIZE_MSG 141 /* Maximum message size allowed */

#define max(A, B) ((A)>=(B) ? (A):(B)) /* Defines the maximum between two values */


typedef struct server
{
	char *name;
	char ip[20];
	int upt;
	int tpt;
	char *siip;
	int sipt;
	int m;
	int r;
	int flag_join; 				/* Used to know when message server registers itself on ID server */
	int flag_first_messages; 	/* Used to know when message server receives the first messages available */
	int flag_finish_read;
	int socket_get_messages;
	int logic_clock;
	int number_messages;
	int socket_tcp;
	char buffer_read[28000];
	char str[17];
	struct sockaddr_in serveraddr_id;
	struct sockaddr_in serveraddr_msg_server_udp;
	struct sockaddr_in serveraddr_msg_server_tcp;
	struct sockaddr_in client_rmb;
	struct _t_lista *ordered_messages;
	struct _t_lista *connected_servers;

}Server;

typedef struct tcp_session
{
	char *name;
	char ip[25];
	int udp_port;
	int tcp_port;
	int socket_id; /* Socket file descriptor */
	char buffer[SIZE_MSG * 200];
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;

}TCP_Session;

typedef struct UDP_Socket
{
	int socket_udp;
	char buffer[28000];
	
}UDP_Socket;

typedef struct message
{
	char *post;
	int logic_time;
}Message;


void establishedUDPsocket(UDP_Socket *udp, Server *msg_server, int function);
void receiveFromRmb(UDP_Socket *rmb, Server *msg_server);
void sendToRmb(UDP_Socket *rmb, Server *msg_server, char *msg);
void sendToIdServer(Server *msg_server, UDP_Socket *udp, char *msg);
void receiveFromIdServer(Server *msg_server, UDP_Socket *udp);
void connectToServersTCP(Server *msg_server, TCP_Session *auxItem, UDP_Socket *rmb, UDP_Socket *udp_id, int *maxfd, fd_set *rfds);
int readTCP(TCP_Session *auxTCP, fd_set *rfds, Server *msg_server);
int writeTCP(TCP_Session *auxTCP, char *msg, unsigned long int msglen, Server *msg_server);
void socketTcpMessageServer(Server *msg_server);
int acceptNewClientsTCP(Server *msg_server, TCP_Session *new_client);
void eliminateNodeFromConnectedServers(Server *msg_server, TCP_Session *auxItem);
void closeSockets(Server *msg_server, UDP_Socket *rmb, UDP_Socket *udp_id);
void freeAllMemory(Server *msg_server, UDP_Socket *rmb, UDP_Socket *udp_id);
void closeAndFree(UDP_Socket *rmb, UDP_Socket *udp_id, Server *msg_server);
void freeAndCloseIni(UDP_Socket *rmb, UDP_Socket *udp_id, Server *msg_server);

#endif
