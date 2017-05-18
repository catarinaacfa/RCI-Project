#ifndef INTERFACE1_H
#define INTERFACE1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "auxi1.h"
#include "servers1.h"

#define SIZE_COMMAND 23 /* Command type size */
#define SIPT 59000 /* ID server default port */
#define INPUT 4

#define SUCCESS 0 /* Message ser is ON */
#define CHECK_PUBLISH 1 /* Checks if publish has been sent */
#define RESEND_MSG 2	/* Message needs to be resend */
#define CHECK_SHOW_LATEST_MESSAGES 3 /* Checks if the messages required were received */
#define TOLERANCE_TIME 3	/* MAX time to wait if publish and show_latest_messages were successful done */


#define max(A, B) ((A)>=(B) ? (A):(B)) /* Maximum between two values */

void read_arguments(int argc, char const* argv[], UDP_Socket *udp_id, Parameters *params);
void select_function(UDP_Socket *udp_id, Parameters *params);
void inicializeRmb(UDP_Socket *udp_id, Parameters *params);
void start(UDP_Socket *udp_id, UDP_Socket *udp_msgserver, Parameters *params);
void show_servers(UDP_Socket *udp_id);
void publish_message(UDP_Socket *udp_msgserver, Parameters *params, int descriptor);
void get_messages(UDP_Socket *udp_msgserver, Parameters *params);
void parseGetServers(UDP_Socket *udp_id);
void parseGetMessages(UDP_Socket *udp_msgserver);
void defineRandomServer(UDP_Socket *udp_id);
void parseDefineServer(UDP_Socket *udp_id, UDP_Socket *udp_msgserver, Parameters *params);
void verifyPublish(UDP_Socket *udp_msgserver, UDP_Socket *udp_id, Parameters *params);

#endif
