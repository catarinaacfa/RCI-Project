#include "interface1.h"

int main(int argc, char const *argv[])
{
	/* Interrupt handler */
	void (*old_handler)(int);
	if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR)exit(1); /* Ignore SIGPIPE */

	/* Defines udp socket to communicate with ID server */
	UDP_Socket *udp_id = (UDP_Socket*)malloc(sizeof(UDP_Socket));
	VerifyMalloc(udp_id);

	/* Auxiliar struct to know what user want */
	Parameters *params = (Parameters*)malloc(sizeof(Parameters));
	VerifyMalloc(params);

	/* Reads optional arguments */
	read_arguments(argc, argv, udp_id, params);

	/* Establishes sockets to communicate with ID server and message server */
	inicializeRmb(udp_id, params);

	/* Main loop */
	select_function(udp_id, params);

	return 0;
}
