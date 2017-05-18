/******************************************************************************
 *
 * File Name: interface.c
 *
 * COMMENTS
 *		implements functions to show the menu to the users
 *
 *****************************************************************************/

#include "interface.h"

/***********************************************************************************************************
 * read_arguments()
 *
 * Arguments: argc - number of arguments
 *			  argv - arguments given by user
 *			  msg_server - struct that contains information about the message server
 *			  rmb - UDP socket established with an rmb terminal
 *			  udp_id - UDP socket established with ID server
 * Returns: (void)
 * Side-Effects: read the arguments given by users to the apllication
 *
 * Description: initializes list
 *
 ***********************************************************************************************************/

void read_arguments(int argc, char const *argv[], Server *msg_server, UDP_Socket *rmb, UDP_Socket *udp_id)
{
 	int i = 1;
 	int j = 0;

 	/* Veryfies if opcional parameters were given by users or not */
 	int arg_siip = 0;
 	int arg_sipt = 0;
 	int arg_m = 0;
 	int arg_r = 0;

 	/* Checks if the required arguments were all given */
 	int count_required_args = 0;

 	/* Allocates memory for message server name */
 	msg_server->name = (char*)malloc(50 * sizeof(char));
	VerifyMalloc(msg_server->name);

	/* Allocates memory for IP of ID server */
	msg_server->siip = (char*)malloc(50 * sizeof(char));
    VerifyMalloc(msg_server->siip);


 	/* Fill in the struct msg_server with parameters given by user */
	while(argv[i] != NULL)
	{
		if(argv[i][j] == '-' && strlen(argv[i]) == 2)
		{
			switch(argv[i][j+1])
			{
				case 'n':
					if(argv[i+1] != NULL && argv[i+1][j] != '-')
					{
						strcpy(msg_server->name, argv[i+1]);
						count_required_args++;
					}
					break;
				case 'j':
					if(argv[i+1] != NULL && argv[i+1][j] != '-')
					{
						sscanf(argv[i+1], "%s", msg_server->ip);
						count_required_args++;
					}
					break;
				case 'u':
					if(argv[i+1] != NULL && argv[i+1][j] != '-')
					{
						sscanf(argv[i+1], "%d", &msg_server->upt);
						count_required_args++;
					}
					break;
				case 't':
					if(argv[i+1] != NULL && argv[i+1][j] != '-')
					{
						sscanf(argv[i+1], "%d", &msg_server->tpt);
						count_required_args++;
					}
					break;	
			}
		}
		else
			count_required_args = -1;

		if(argv[i][j] == '-' && strlen(argv[i]) == 2)
		{
			switch(argv[i][j+1])
			{
				case 'i':
                    if(argv[i+1] != NULL && argv[i+1][j] != '-' && arg_siip == 0)
                    {
                    	strcpy(msg_server->siip, argv[i+1]);
						arg_siip = 1;
                    }
                    else if(arg_siip == 1)
                    {
                    	printf("Parameter siip already defined");
                    	count_required_args = -1;
                    }
					break;
				case 'p':
					if(argv[i+1] != NULL && argv[i+1][j] != '-' && arg_sipt == 0)
					{
						sscanf(argv[i+1], "%d", &msg_server->sipt);
						arg_sipt = 1;
					}
					else if(arg_sipt == 1)
                    {
                    	printf("Parameter sipt already defined");
                    	count_required_args = -1;
                    }
					break;
				case 'm':
					if(argv[i+1] != NULL && argv[i+1][j] != '-' && arg_m == 0)
					{
						sscanf(argv[i+1], "%d", &msg_server->m);
						arg_m = 1;
					}
					else if(arg_m == 1)
                    {
                    	printf("Parameter m already defined");
                    	count_required_args = -1;
                    }
					break;
				case 'r':
					if(argv[i+1] != NULL && argv[i+1][j] != '-' && arg_r == 0)
					{
						sscanf(argv[i+1], "%d", &msg_server->r);
						arg_r = 1;
					}
					else if(arg_r == 1)
                    {
                    	printf("Parameter r already defined");
                    	count_required_args = -1;
                    }
					break;
			}
		}
		else
			count_required_args = -1;

		j=0;
		i=i+2;
	}

	/* Exit the program if the required arguments weren't given  by user*/
	if(count_required_args != 4)
	{
		printf("\nParameters are wrong!\nTo start the application you need to give the following command (the arguments between [] are opcional):\n");
		printf("\nmsgserv –n name –j ip -u upt –t tpt [-i siip] [-p sipt] [–m m] [–r r]\n\n");
		freeAndCloseIni(rmb, udp_id, msg_server);
		exit(EXIT_FAILURE);
	}

	/* In case the user doesn't give the optional arguments */
	if(arg_siip == 0)
		strcpy(msg_server->siip, "tejo.tecnico.ulisboa.pt");
	if(arg_sipt == 0)
		msg_server->sipt = SIPT;
	if(arg_m == 0)
		msg_server->m = M;
	if(arg_r == 0)
		msg_server->r = R;

}

/******************************************************************************************************
 * select_function()
 *
 * Arguments: msg_server - message server
 *            udp_id - udp session established with ID server
 *            rmb - udp session established with rmb server
 * Returns: none
 * Side-Effects: allows communication between rmb, id server and message servers
 *
 * Description: core cycle of the program, checks which sockets are ready to be read and process them
 *
 *****************************************************************************************************/

void select_function(UDP_Socket *udp_id, UDP_Socket *rmb, Server *msg_server)
{
	/* File decriptor list */
	fd_set rfds;

	int maxfd = 0;
	int counter = 0;
	int i = 0;
	struct timeval tv;
	time_t interval = 0;

	tv.tv_sec = msg_server->r;
	tv.tv_usec = 0;	

	while(1)
	{
    	FD_ZERO(&rfds);

    	FD_SET(msg_server->socket_tcp, &rfds);
    	maxfd = msg_server->socket_tcp;

    	/* Adds all file descriptors to rfds list */
    	setFileDescriptors(&rfds, udp_id, rmb, msg_server, &maxfd);

    	/* Updates the time with the seconds remaining after processing the file descriptors */
		tv.tv_sec = tv.tv_sec - interval;
		tv.tv_usec = 0;

		if(tv.tv_sec < 0)
			tv.tv_sec = 0;

		counter = select(maxfd + 1, &rfds, NULL, NULL, &tv);

		/* Used to count the time that was spent inside for loop, when some/all of the file descriptors ready are processed */
    	interval = time(NULL);

		if(counter < 0)
		{
			if(errno != EINTR && errno != EPIPE)
			{
				printf("Error: %s\n %d", strerror(errno), errno);
				closeAndFree(rmb, udp_id, msg_server);
				exit(1);
			}

		}
		if(counter == 0)
		{
			/* Message Server just refresh itself, after r seconds, on ID server if he had joined already */
			if(msg_server->flag_join != NOTDONE)
				join(msg_server, udp_id);
				

			/* Updates the time to r seconds, when r seconds passed */
			tv.tv_sec = msg_server->r;
			tv.tv_usec = 0;
			interval = 0;
		}
		else
		{
			/* Process all file descriptors ready to be read */
			for(i = 0; i <= maxfd; i++)
			{
				if(FD_ISSET(i, &rfds))
				{
					if(i == msg_server->socket_tcp)
						acceptNewConnections(&rfds, &maxfd, msg_server, udp_id);
					else if(i == 0)
						start(msg_server, udp_id, rmb);
					else if(i == rmb->socket_udp)
					{
						receiveFromRmb(rmb, msg_server);
						parseRmbCommand(rmb, msg_server);
					}
					else if(i == udp_id->socket_udp)
					{
						parseGetServers(msg_server, udp_id, rmb, &maxfd, &rfds);

						/* Get all messages that message servers have */
						if(msg_server->flag_first_messages == NOTDONE)
							get_messages(msg_server);
					}
					else
						readMessageFromMessageServer(msg_server, i, &rfds);
				}
			}

			/* Time spent processing the file descriptors */
      		interval = difftime(time(NULL), interval);
		}
	}
}

/**************************************************************************************************************
 * setFileDescriptors()
 *
 * Arguments: msg_server - message server
 *            udp_id - udp session established with ID server
 *            rmb - udp session established with rmb server
 *			  rfds - rfds file descriptor list
 *			  maxfd - max number of file descriptors set in rfds
 * Returns: none
 * Side-Effects: set rfds file descriptor list adding the UDP and TCP sockets, that are already established
 *
 * Description: prepares file descriptor list to do the select() function
 *
 **************************************************************************************************************/

void setFileDescriptors(fd_set *rfds, UDP_Socket *udp_id, UDP_Socket *rmb, Server *msg_server, int *maxfd)
{
	t_lista *aux;
	TCP_Session *auxItem = NULL;
	int temp_max = 0;

	aux = msg_server->connected_servers;

	/* Adds stdin and UDP sockets to the file descriptor list */
	FD_SET(udp_id->socket_udp, rfds);
	FD_SET(rmb->socket_udp, rfds);
	FD_SET(0, rfds);

	/* Updates the maximum of file descriptors */
	temp_max = max(*maxfd, udp_id->socket_udp);
	*maxfd = max(temp_max, rmb->socket_udp);

	if(aux != NULL)
	{
		auxItem = getListItem(aux);

		/* Puts all TCP sessions in rfds and defines the maximum file descriptor */
		while(aux != NULL && auxItem->socket_id > 0)
		{
			FD_SET(auxItem->socket_id, rfds);

			*maxfd = max(auxItem->socket_id, *maxfd);

			aux = getNextListElement(aux);

            if(aux != NULL)
                auxItem = getListItem(aux);
		}
	}
}

/***************************************************************************************************
 * acceptNewConnections()
 *
 * Arguments: msg_server - message server
 *			  rfds - rfds file descriptor list
 *			  maxfd - max number of file descriptors set in rfds
 *			  udp_id - UDP socket established with ID server
 * Returns: none
 * Side-Effects: accepts new TCP sessions
 *
 * Description: establishes new connections with clients that asks to connect with message server
 *
 **************************************************************************************************/

void acceptNewConnections(fd_set *rfds, int *maxfd, Server *msg_server, UDP_Socket *udp_id)
{
	int connected = 0;

	TCP_Session *new_client = (TCP_Session*)malloc(sizeof(TCP_Session));
	VerifyMalloc(new_client);

	connected = acceptNewClientsTCP(msg_server, new_client);

	if(connected > 0)
	{
		msg_server->connected_servers = newNode(msg_server->connected_servers, (Item)new_client);

		strcpy(new_client->ip, inet_ntoa(new_client->clientaddr.sin_addr));
		new_client->tcp_port = ntohs(new_client->clientaddr.sin_port);
		new_client->name = NULL;
	}
	else
	{
		itemFree(new_client);
		free(new_client);
	}

}

/*********************************************************************************************************
 * start()
 *
 * Arguments: msg_server - message server
 *			  udp_id - UDP socket established with ID server
 *			  rmb - UDP socket established with rmb terminal
 * Returns: none
 * Side-Effects: checks the stdin
 *
 * Description: checks what was the input given by user
 *
 *********************************************************************************************************/

void start(Server *msg_server, UDP_Socket *udp_id, UDP_Socket *rmb)
{
	char *option = (char*)malloc(20 * sizeof(char));

	scanf("%s%*c", option); /* %*c is used to discard the next character (enter) */

	if(strcmp(option, "join") != 0 && strcmp(option, "show_servers") != 0 && strcmp(option, "show_messages") != 0 && strcmp(option, "exit") != 0)
		printf("\nInvalid command, please insert a valid option!\n\n");

	if(strcmp(option, "join") == 0) /* Allows user to resgitry itself on ID server */
		join(msg_server, udp_id);
	else if(strcmp(option, "show_servers")==0) /* Shows all servers that have a TCP session established with itself */
		show_servers(msg_server);
	else if(strcmp(option, "show_messages")==0) /* Shows all messages */
		show_messages(msg_server);
	else if(strcmp(option, "exit") == 0) /* Exits apllication */
	{
		/* Close all sockets and frees all memory allocated */
		closeAndFree(rmb, udp_id, msg_server);
		free(option);
		exit(0);
	}

	free(option);
}

/**************************************************************************************************************
 * inicializeServer()
 *
 * Arguments: msg_server - server to initialize
 *            udp_id - UDP socket established with ID server
 *			  rmb - UDP socket established with rmb terminal
 * Returns: none
 * Side-Effects: initializes parameters of msg_server and send a message to ID server
 *
 * Description: initializes lists and variables that msg_server contains before being registered (before join)
 *
 **************************************************************************************************************/

void initializeServer(Server *msg_server, UDP_Socket *udp_id, UDP_Socket *rmb)
{
	/* Inicializes the server logic clock */
 	msg_server->logic_clock = 0;

 	/* Flag to know if message server has already registered itslef on server ID */
 	msg_server->flag_join = NOTDONE;

 	/* Flag to know if first messages were received */
 	msg_server->flag_first_messages = NOTDONE;

 	msg_server->flag_finish_read = DONE;

 	/* Used to hold the socket choosen to get the first messages */
 	msg_server->socket_get_messages = -1;

 	/* Inicializes the number of messages that server has */
 	msg_server->number_messages = 0;

 	/* Inicializes the messages and the connected servers list */
 	msg_server->ordered_messages = iniList();
 	msg_server->connected_servers = iniList();

 	/* Establish an udp socket to communicate with ID server */
 	udp_id->socket_udp = 0;
 	establishedUDPsocket(udp_id, msg_server, CLIENT);

 	/* Establish an udp socket to communicate with rmb */
 	establishedUDPsocket(rmb, msg_server, SERVER);

 	/* Makes message server available to be a TCP server */
 	socketTcpMessageServer(msg_server);

}

/*********************************************************************************************************************************
 * parseGetServers()
 *
 * Arguments: msg_server - message server
 *            udp_id - UDP socket established with ID server
 *			  rmb - UDP socket established with rmb terminal
 *			  maxfd - max file descriptor
 *			  rfds - file descriptors list
 * Returns: (void)
 * Side-Effects: parse the answer given by ID server
 *
 * Description: inicializes the message server, creates a list with the servers that are already registered on the ID server and
 *				checks if message server was registered
 *
 ********************************************************************************************************************************/

void parseGetServers(Server *msg_server, UDP_Socket *udp_id, UDP_Socket *rmb, int *maxfd, fd_set *rfds)
{
	int j=0, count=0;

	char name[25];
	char ip[25];
	int upt;
	int tpt;
	char *aux;

	/* Used to parse the message received from ID server */
	char *serverInfo = (char*)malloc(5000 * (sizeof(char)));
	VerifyMalloc(serverInfo);

	/* Auxiliar pointer to be able to frees the memory */
	aux = serverInfo;

	/* Receives all servers registered on ID server */
	receiveFromIdServer(msg_server, udp_id);

 	strcpy(serverInfo, udp_id->buffer);
 	serverInfo=&serverInfo[8];

 	/* Creates a list of all servers that the server has to establish a TCP connection */
 	while(strlen(serverInfo) > 0)
 	{
 		sscanf(serverInfo, "%[^;];%[^;];%d;%d", name, ip, &upt, &tpt);

 		/* Don't put itself on the list */
 		if(strcmp(msg_server->ip, ip) != 0 || (strcmp(msg_server->ip, ip) == 0 && tpt != msg_server->tpt && upt != msg_server->upt))
 		{
 			/* Adds new servers to the connected servers list that are registered on ID server */
 			if(msg_server->flag_join != CHECK_JOIN)
 			{
 				TCP_Session *new_node = (TCP_Session*)malloc(sizeof(TCP_Session));
		 		VerifyMalloc(new_node);

		 		msg_server->connected_servers = newNode(msg_server->connected_servers, (Item)new_node);

				new_node->name = (char*)malloc((strlen(name) +1) * (sizeof(char)));
				VerifyMalloc(new_node->name);
				strcpy(new_node->name, name);

				strcpy(new_node->ip, ip);
				new_node->udp_port = upt;
				new_node->tcp_port = tpt;
				new_node->socket_id = -1;
 			}
 		}
 		else
 			count++;
 
		/* Search fot the next '\n' e move the pointer there */
		for (j=0; serverInfo[j] != '\n' && j < strlen(serverInfo); j++){
		}

		if(j < strlen(serverInfo))
			serverInfo=&serverInfo[j+1];
	}

	if(msg_server->flag_join == NOTDONE)
		establishTCPsession(msg_server, rmb, udp_id, maxfd, rfds); /* Connect to servers that are already registered */
	
	/* Means that message server is registered on ID server */
	if(count == 1)
		msg_server->flag_join = SUCCESS;
	else
		join(msg_server, udp_id); /* If flag_join wasn't successful after check at parseGetServers, join again */
							
	free(aux);
}

/*****************************************************************************************************
 * parse_rmb_command()
 *
 * Arguments: msg_server - message server
 *            rmb - UDP socket established with rmb server
 * Returns: none
 * Side-Effects: compare to possible requests of rmb server (publish or get messages)
 *
 * Description: in case of rmb wants to communicate with message server, check what he wants to send
 *
 *****************************************************************************************************/

void parseRmbCommand(UDP_Socket *rmb, Server *msg_server)
{
	char aux1[15];
	char *aux2 = "";
	char *msg = "";

	sscanf(rmb->buffer, "%s ", aux1);

	aux2=&(rmb->buffer[strlen(aux1)+1]);

	/* Checks the buffer content received from a rmb terminal */
	if(strcmp(aux1, "PUBLISH") == 0)
	{
		addMessageFromRmb(msg_server, aux2); /* Adds the message received by a terminal to its message list */
		replicateMessage(msg_server, aux2); /* Replicates the message received to all servers that have a TCP session established with it */
	}
	else if(strcmp(aux1, "GET_MESSAGES") == 0)
	{
		makeMessageList(atoi(aux2), msg_server, &msg, RMB); /* Make a string with all messages requested by rmb terminal */
		sendToRmb(rmb, msg_server, msg); /* Send it to rmb */
		free(msg);
	}

}

/******************************************************************************
 * join()
 *
 * Arguments: msg_server - message server
 *            udp_id - UDP socket established with ID server
 * Returns: (void)
 * Side-Effects: registry of message server in the ID server
 *
 * Description: allows user to registry the message server in the ID server
 *
 *****************************************************************************/

void join(Server *msg_server, UDP_Socket *udp_id)
{
	char *registry = (char*)malloc(100 * sizeof(char));
	VerifyMalloc(registry);

	/* Message sent every r seconds to registry message server on ID server */
	sprintf(registry, "REG %s;%s;%d;%d", msg_server->name, msg_server->ip, msg_server->upt, msg_server->tpt);

	sendToIdServer(msg_server, udp_id, registry);

	/* After doing join, asks ID server who is already resgitered in order to be able to establish a TCP session with them */
 	sendToIdServer(msg_server, udp_id, "GET_SERVERS");

 	if(msg_server->flag_join != NOTDONE)
 		msg_server->flag_join = CHECK_JOIN;

	free(registry);
}

/***********************************************************************************************************
 * establishTCPsession()
 *
 * Arguments: msg_server - message server
 *			  rmb - UDP socket established with rmb terminal
 *			  udp_id - UDP socket established with ID server
 *			  maxfd - max file descriptor
 *			  rfds - file descriptors list
 * Returns: (void)
 * Side-Effects: establish all TCP connections
 *
 * Description: connect the message server to all servers previously registered in ID server
 *
 ***********************************************************************************************************/

void establishTCPsession(Server *msg_server, UDP_Socket *rmb, UDP_Socket *udp_id, int *maxfd, fd_set *rfds)
{
	t_lista *auxiliar;
	TCP_Session *auxItem;

	auxiliar = msg_server->connected_servers;

	/* Goes through the connected servers list and tries to establish a TCP session with all of them */
	while(auxiliar != NULL)
	{
		auxItem = getListItem(auxiliar);
		connectToServersTCP(msg_server, auxItem, rmb, udp_id, maxfd, rfds);
		auxiliar = getNextListElement(auxiliar);
	}

}

/********************************************************************************************************************
 * show_servers()
 *
 * Arguments: msg_server - message server
 * Returns: (void)
 * Side-Effects: show all active TCP sessions between message servers
 *
 * Description: allows user to know all sessions between his server and the others message servers (TCP sessions)
 *
 *******************************************************************************************************************/

void show_servers(Server *msg_server)
{
	t_lista* aux;
	TCP_Session *auxItem;

	aux = msg_server->connected_servers;

	if(aux == NULL)
		printf("\nNo TPC sessions established\n\n");

	/* Goes through connected servers list and prints all of them */
	while(aux != NULL)
	{
		auxItem = getListItem(aux);
		if(auxItem->socket_id > 0)
		{
			if(auxItem->name == NULL)
				printf("%s %d \n", auxItem->ip, auxItem->tcp_port);
			else
				printf("%s %s %d %d\n", auxItem->name, auxItem->ip, auxItem->udp_port, auxItem->tcp_port);
		}

		aux = getNextListElement(aux);
	}
}

/**************************************************************
 * show_messages()
 *
 * Arguments: msg_server - message server
 * Returns: (void)
 * Side-Effects: shows all messages saved on message server
 *
 * Description: shows all messages to user
 *
 *************************************************************/

void show_messages(Server *msg_server)
{
	t_lista* aux;
	Message *auxItem;

	aux = msg_server->ordered_messages;

	if(aux == NULL)
		printf("\nThere aren't any messages yet\n\n");

	while(aux != NULL)
	{
		auxItem = getListItem(aux);

		printf("%s\n", auxItem->post);

		aux = getNextListElement(aux);
	}

}

/*********************************************************************************************
 * get_messages()
 *
 * Arguments: msg_server - message server
 * Returns: (void)
 * Side-Effects: obtain all messages that messages servers have after joining into ID server
 *
 * Description: allows user to get all messages after joined
 *
 ********************************************************************************************/

void get_messages(Server *msg_server)
{
	t_lista *aux;
	int i = 0;

	TCP_Session *auxTCP = NULL;

	aux = msg_server->connected_servers;

	if(aux != NULL)
	{
		auxTCP = getListItem(aux);
		
		do
		{
			i = writeTCP(auxTCP, "SGET_MESSAGES\n", strlen("SGET_MESSAGES\n"), msg_server);
			if(i > -1)
				msg_server->socket_get_messages = auxTCP->socket_id;
			else
			{
				aux = getNextListElement(aux);
				if(aux != NULL)
					auxTCP = getListItem(aux);
			}

		}while(i < 0 && aux != NULL);
	}
	
	if(aux == NULL)
		msg_server->flag_first_messages = DONE;
}

/**************************************************************************************************************
 * addMessageFromRmb()
 *
 * Arguments: msg_server - message server
 *			  msg - new message published by a terminal
 * Returns: (void)
 * Side-Effects: adds a new message to the message server
 *
 * Description: message server receives a new message from a rmb terminal and adds it to its list of messages
 *
 **************************************************************************************************************/

void addMessageFromRmb(Server *msg_server, char *msg)
{
	if(strlen(msg) <= SIZE_MSG)
	{
		/* Check if message server has reached the max number of messages allowed and in that case, eliminates the older one */
		if(msg_server->number_messages == msg_server->m)
		{
			msg_server->ordered_messages = deleteFirstNode(msg_server->ordered_messages, MSG);
			msg_server->number_messages = msg_server->number_messages - 1;
		}

		Message *new_message = (Message*)malloc(sizeof(Message));
	 	VerifyMalloc(new_message);

	 	/* Adds the new message to the list */
	 	msg_server->ordered_messages = newEndNode(msg_server->ordered_messages, (Item)new_message);

	 	new_message->post = (char*)malloc((strlen(msg)+1) * (sizeof(char)));
		VerifyMalloc(new_message->post);
		
		strcpy(new_message->post, msg);

		if(new_message->post[strlen(new_message->post)-1] == '\n')
			new_message->post[strlen(new_message->post)-1] = '\0';

		new_message->logic_time = msg_server->logic_clock + 1;
		
		/* Update number of messages */
		msg_server->number_messages = msg_server->number_messages + 1;

		/* Update clock after publish */
		msg_server->logic_clock = msg_server->logic_clock + 1;
	}
	else
		printf("\nCan't add a new message from rmb: it has more than 140 characters\n");
	
}

/********************************************************************************************
 * makeMessageList()
 *
 * Arguments: msg_server - message server
 *			  whoAsked - holds who asked for messages (rmb or another message server)
 *			  number - number of messages required
 *			  msg - string that contains all messages required
 * Returns: (void)
 * Side-Effects: creates a string to send with all messages
 *
 * Description: prepares a string with all messages
 *
 *******************************************************************************************/

void makeMessageList(int number, Server *msg_server, char **msg, int whoAsked)
{
	t_lista *aux;
	Message *auxItem;
	char list_messages[number * SIZE_MSG + 10];
	int i = 0;
	char message_clock[4];
	int until_Number_Required = msg_server->number_messages - number;

	aux = msg_server->ordered_messages;

	/* Checks who ask for messages in order to make the right answer (depending on who asked) */
	if(whoAsked == RMB)
		strcpy(list_messages,"MESSAGES\n");
	else if(whoAsked == MSG)
		strcpy(list_messages,"SMESSAGES\n");

	while(aux != NULL)
	{
		auxItem = getListItem(aux);

		if(i >= until_Number_Required)
		{
			/* In case who asked was a message server, we have to send the logic time of every message */
			if(whoAsked == MSG)
			{
				sprintf(message_clock, "%d", auxItem->logic_time);
				strcat(list_messages, message_clock);
				strcat(list_messages, ";");
			}

			strcat(list_messages, auxItem->post);
			strcat(list_messages, "\n");
		}

		aux = getNextListElement(aux);
		i++;
	}

	if(whoAsked == MSG)
		strcat(list_messages, "\n"); /* Messages sent to a message server needs a final \n */

	/* Message ready to send */
	*msg = (char*)malloc((strlen(list_messages)+1)* sizeof(char));
	strcpy(*msg, list_messages);

}

/*****************************************************************************************
 * readMessageFromMessageServer()
 *
 * Arguments: msg_server - message server
 *			  right_socket - socket ready to be read
 *			  rfds - rfds file descriptor list
 * Returns: (void)
 * Side-Effects: read a message send by another message server and send back the answer
 *
 * Description: checks if can read from that socket and, in that case, builds the answer
 *
 ****************************************************************************************/

void readMessageFromMessageServer(Server *msg_server, int right_socket, fd_set *rfds)
{
	t_lista *aux;
	TCP_Session *auxItem;

	/* Holds the position in the list of the node that has the socket ready, in case we have to delete it (if it has to be closed) */
	int position_in_list = 1;

	aux = msg_server->connected_servers;
	auxItem = getListItem(aux);

	
	int i = 0;

	/* Check which is the ready socket, to know all information about which is the TCP session */
	while((aux != NULL) && (auxItem->socket_id != right_socket))
	{
		aux = getNextListElement(aux);
		auxItem = getListItem(aux);
		position_in_list++;
	}

	
	if(readTCP(auxItem, rfds, msg_server) <= 0)
	{
		/* If the socket was closed, remove the connection from the list */
		if(position_in_list == 1)
			msg_server->connected_servers = deleteFirstNode(msg_server->connected_servers, NAME);
		else
			msg_server->connected_servers = deleteMiddleNode(msg_server->connected_servers, position_in_list);

		if(msg_server->flag_first_messages == NOTDONE)
			get_messages(msg_server);

	}
	else
	{
		if(msg_server->flag_finish_read == DONE)
		{
			memset(&msg_server->str, 0, sizeof(msg_server->str));

			/* Checks which was sent by a message server */
			while(msg_server->buffer_read[i] != '\n')
			{
				msg_server->str[i] = msg_server->buffer_read[i];
				i++;
			}
			memset(&auxItem->buffer, 0, sizeof(auxItem->buffer));
			strcat(msg_server->str, "\n");
		}

		if((msg_server->flag_first_messages == NOTDONE && msg_server->socket_get_messages == auxItem->socket_id) || msg_server->flag_first_messages == DONE)
		{
			/* Checks if the message received makes part of the protocol */
			if(strcmp(msg_server->str, "SGET_MESSAGES\n") == 0)
				sendMessagesToMessageServer(msg_server, auxItem); /* Send all messages contained in ordered messages list */
			else if(strcmp(msg_server->str, "SMESSAGES\n") == 0)
			{
				if((auxItem->buffer[strlen(auxItem->buffer) -1] != '\n') && (auxItem->buffer[strlen(auxItem->buffer) - 2] != '\n'))
					msg_server->flag_finish_read = NOTDONE;

				if(msg_server->flag_finish_read == NOTDONE)
					strcat(auxItem->buffer, msg_server->buffer_read);

				if((auxItem->buffer[strlen(auxItem->buffer) -1] == '\n') && (auxItem->buffer[strlen(auxItem->buffer) - 2] == '\n'))
				{
					parseReceivedMessages(auxItem, msg_server); /* Parse the messages received by another message server */
					msg_server->flag_finish_read = DONE;
					msg_server->flag_first_messages = DONE;
				}
				else
					msg_server->flag_finish_read = NOTDONE;
			}
			
			
		}	
	}
}

/***************************************************************************************************************
 * sendMessagesToMessageServer()
 *
 * Arguments: msg_server - message server
 *			  aux - contains all information needed about the client
 * Returns: (void)
 * Side-Effects: checks what message was sent and if it's corresponds to the protocol, writes back the answer
 *
 * Description: gives the answer to the request, if the resquest was valid
 *
 **************************************************************************************************************/

void sendMessagesToMessageServer(Server *msg_server, TCP_Session* aux)
{
	char *msg = "";

	/* Prepares the string with all messages */
	makeMessageList(msg_server->number_messages, msg_server, &msg, MSG);

	writeTCP(aux, msg, strlen(msg), msg_server);

	free(msg);
}

/***************************************************************************************************************
 * parseReceivedMessages()
 *
 * Arguments: msg_server - message server
 *			  auxTCP - contains the buffer to parse
 * Returns: (void)
 * Side-Effects: adds messages to the message list
 *
 * Description: after join, the message server has to ask for the messages that are already in the others
 *
 **************************************************************************************************************/

void parseReceivedMessages(TCP_Session *auxTCP, Server *msg_server)
{
	int message_time = 0;
	int j = 0;

	char *parseAux;
	char message[SIZE_MSG];

	parseAux = (char*)malloc((strlen(auxTCP->buffer)+1) * sizeof(char));
	VerifyMalloc(parseAux);

	strcpy(parseAux, auxTCP->buffer);
    char *aux = parseAux;
 
 	if(strlen(parseAux)>11)
    {
 		parseAux=&parseAux[10];

	 	/* Creates a list of all messages received */
	 	while(strlen(parseAux) > 1)
	 	{
			/* Clean message */
			memset(&message, 0, sizeof(message));

	 		/* Checks if message size is valid */
	 		if(strlen(message) <= SIZE_MSG)
	 		{
				sscanf(parseAux, "%d;%[^\n]", &message_time, message);

	 			Message *new_message = (Message*)malloc(sizeof(Message));
		 		VerifyMalloc(new_message);

				/* Check if message server has reached the max number of messages allowed and in that case, eliminates the older one */
				if(msg_server->number_messages == msg_server->m)
				{
					msg_server->ordered_messages = deleteFirstNode(msg_server->ordered_messages, MSG);
					msg_server->number_messages = msg_server->number_messages - 1;
				}

		 		/* Adds a message to the messages list */
		 		msg_server->ordered_messages = newEndNode(msg_server->ordered_messages, (Item)new_message);

				new_message->post = (char*)malloc((strlen(message)+1) * (sizeof(char)));
				VerifyMalloc(new_message->post);

				strcpy(new_message->post, message);
				new_message->logic_time = message_time;

				/* Update logic clock */
				msg_server->logic_clock = max(msg_server->logic_clock, new_message->logic_time) + 1;

				/* Update number of messages */
				msg_server->number_messages = msg_server->number_messages + 1;
	 		}

			/* Search fot the next '\n' e move the pointer there */
			for (j=0; parseAux[j] != '\n' && j < strlen(parseAux); j++){
			}

			if(j < strlen(parseAux))
				parseAux=&parseAux[j+1];
		}
	}

	memset(&auxTCP->buffer, 0, sizeof(auxTCP->buffer));

	free(aux);
}

/***************************************************************************************************************
 * replicateMessage()
 *
 * Arguments: msg_server - message server
 *			  msg - message to replicate
 * Returns: (void)
 * Side-Effects: replicates a message published by one terminal
 *
 * Description: send to all message servers that are available a message sent by rmb
 *
 **************************************************************************************************************/

void replicateMessage(Server *msg_server, char *msg)
{
	t_lista *aux;
	TCP_Session *auxTCP = NULL;

	char str[5];

	sprintf(str, "%d", msg_server->logic_clock);

	char aux_message[strlen(msg) + strlen(str) + 11];

	strcpy(aux_message, "SMESSAGES\n");
	strcat(aux_message, str);
	strcat(aux_message, ";");
	strcat(aux_message, msg);
	strcat(aux_message, "\n");

	aux = msg_server->connected_servers;

	while(aux != NULL)
	{
		auxTCP = getListItem(aux);

		writeTCP(auxTCP, aux_message, strlen(aux_message), msg_server);

		aux = getNextListElement(aux);
	}

}

