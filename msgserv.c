#include "interface.h"

int main(int argc, char const *argv[])
{
	/* Interrupt handler */
	void (*old_handler)(int);
	if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR)exit(1); /* Ignore SIGPIPE */

	/* Struct that holds all information about message server */
	Server *msg_server = (Server*)malloc(sizeof(Server));
	VerifyMalloc(msg_server);

	/* Socket UDP for ID */
	UDP_Socket *udp_id = (UDP_Socket*)malloc(sizeof(UDP_Socket));
	VerifyMalloc(udp_id);

	/* Socket UDP for rmb */
	UDP_Socket *rmb = (UDP_Socket*)malloc(sizeof(UDP_Socket));
	VerifyMalloc(rmb);

	/* Prepares message server to select function */
	read_arguments(argc, argv, msg_server, rmb, udp_id);
	initializeServer(msg_server, udp_id, rmb);

	/* Main Loop */
	select_function(udp_id, rmb, msg_server);

	return 0;
}
