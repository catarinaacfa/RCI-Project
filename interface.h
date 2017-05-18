#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>

#include "servers.h"

#define SIPT 59000 /* Default value for SIPT - IP of ID server */
#define M 200 /* Default value for m - max messages that message server can save */
#define R 10 /* Default value for r - time to refresh the registry on ID server */

#define NOTDONE 2
#define DONE 3

 
#define CHECK_JOIN 0
#define SUCCESS 1


void read_arguments(int argc, char const* argv[], Server *msg_server, UDP_Socket *rmb, UDP_Socket *udp_id);
void select_function(UDP_Socket *udp_id, UDP_Socket *rmb, Server *msg_server);
void setFileDescriptors(fd_set *rfds, UDP_Socket *udp_id, UDP_Socket *rmb, Server *msg_server, int *maxfd);
void acceptNewConnections(fd_set *rfds, int *maxfd, Server *msg_server, UDP_Socket *udp_id);
void start(Server *msg_server, UDP_Socket *udp_id, UDP_Socket *rmb);
void initializeServer(Server *msg_server, UDP_Socket *udp, UDP_Socket *rmb);
void parseGetServers(Server *msg_server, UDP_Socket *udp_id, UDP_Socket *rmb, int *maxfd, fd_set *rfds);
void parseRmbCommand(UDP_Socket *rmb, Server *msg_server);
void join(Server *msg_server, UDP_Socket *udp);
void establishTCPsession(Server *msgserv, UDP_Socket *rmb, UDP_Socket *udp_id, int *maxfd, fd_set *rfds);
void show_servers(Server *msg_server);
void show_messages(Server *msg_server);
void get_messages(Server *msg_server);
void addMessageFromRmb(Server *msg_server, char *msg);
void makeMessageList(int number, Server *msg_server, char **msg, int whoAsked);
void readMessageFromMessageServer(Server *msg_server, int right_socket, fd_set *rfds);
void sendMessagesToMessageServer(Server *msg_server, TCP_Session *aux);
void parseReceivedMessages(TCP_Session *auxTCP, Server *msg_server);
void replicateMessage(Server *msg_server, char *msg);

#endif
