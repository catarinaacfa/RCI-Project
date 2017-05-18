/******************************************************************************
 *
 * File Name: interface1.c
 *
 * COMMENTS
 *		implements functions related to handling inputs
 *
 *****************************************************************************/

#include "interface1.h"

/******************************************************************************
 * read_arguments()
 *
 * Arguments: argc - number of arguments
 *			  argv - arguments given by user
 *			  udp_id - struct that contains information about the id server
 * Returns: (void)
 * Side-Effects: read the arguments given by users to the apllication
 *
 * Description: initializes list
 *
 *****************************************************************************/

void read_arguments(int argc, char const *argv[], UDP_Socket *udp_id, Parameters *params)
{
 	int i = 1;
 	int j = 0;

 	/* To verify if opcional parameters were given by users or not */
 	int arg_serv_ip = 0;
 	int arg_serv_port = 0;

 	/* Allocates memory for IP of ID server */
 	udp_id->serv_ip = (char*)malloc(50 * sizeof(char));
    VerifyMalloc(udp_id->serv_ip);

 	/* Fill in the struct msg_server with parameters given by user */
	while(argv[i] != NULL)
	{
    	/* Checks argv flags and sets ip and port if needed */
		if(argv[i][j] == '-' && strlen(argv[i]) == 2)
		{
			switch(argv[i][j+1])
			{
				case 'i':
                    if(argv[i+1] != NULL && argv[i+1][j] != '-' && arg_serv_ip == 0)
                    {
                    	strcpy(udp_id->serv_ip, argv[i+1]);
						arg_serv_ip = 1;
                    }
                    else if(arg_serv_ip == 1)
                    {
                    	printf("\nParameter siip already defined\nTo start the application you need to give the following command (the arguments between [] are opcional):\n");
						printf("\nrmb [-i siip] [-p sipt]\n\n");
						freeAllMemory(udp_id, params);
                    	exit(EXIT_FAILURE);
     
                    }
					break;
				case 'p':
					if(argv[i+1] != NULL && argv[i+1][j] != '-' && arg_serv_port == 0)
					{
						sscanf(argv[i+1], "%d", &udp_id->serv_port);
						arg_serv_port = 1;
					}
					else if(arg_serv_port == 1)
                    {
                    	printf("\nParameter sipt already defined\nTo start the application you need to give the following command (the arguments between [] are opcional):\n");
						printf("\nrmb [-i siip] [-p sipt]\n\n");
						freeAllMemory(udp_id, params);
                    	exit(EXIT_FAILURE);
                    }
					break;
			}
		}
		else
		{
			printf("\nInsert valid parameters\nTo start the application you need to give the following command (the arguments between [] are opcional):\n");
			printf("\nrmb [-i siip] [-p sipt]\n\n");
			freeAllMemory(udp_id, params);
        	exit(EXIT_FAILURE);
		}
		j=0;
		i=i+2;
	}

  	/* If no flags were set, it sets the ip and port with default arguments */
	if(arg_serv_ip == 0)
		strcpy(udp_id->serv_ip, "tejo.tecnico.ulisboa.pt");
	if(arg_serv_port == 0)
		udp_id->serv_port = SIPT;
}

/**************************************************************************************************
 * select_function()
 *
 * Arguments: udp_id - udp socket established with ID server
 *			  udp_msgserver - udp socket established with a message server
 *			  params - parameters given by user if the option is publish or show_latest_messages
 * Returns: (void)
 * Side-Effects: main loop of the program, which manage created sockets
 *
 * Description: checks which sockets are ready to be read in order to process them
 *
 *************************************************************************************************/

void select_function(UDP_Socket *udp_id, Parameters *params)
{
	fd_set rfds;
	int maxfd, counter;

	/* Defines udp socket to communicate with message server */
	UDP_Socket *udp_msgserver = (UDP_Socket*)malloc(sizeof(UDP_Socket));
	VerifyMalloc(udp_msgserver);

	udp_msgserver->serv_ip = NULL;
	udp_msgserver->serv_port = 0;
	udp_msgserver->socket_udp = -1;
	udp_msgserver->flag_available = 0;

	udp_msgserver->flag_check_publish = 0;
	udp_msgserver->flag_check_show_msg = 0;

	udp_msgserver->start_time_check.tv_sec = 0;
	udp_msgserver->start_time_check.tv_usec = 0;

	struct timeval timeToCheck;

	timeToCheck.tv_sec = 2;
	timeToCheck.tv_usec = 0;


   	/* Main loop */
	while(1)
	{
		FD_ZERO(&rfds);
    
    	/* Adds file descriptor to a fd_set */
    	FD_SET(0, &rfds);
		FD_SET(udp_id->socket_udp, &rfds);
		if(udp_msgserver->flag_available == 1)
		{
			FD_SET(udp_msgserver->socket_udp, &rfds);
			maxfd=max(udp_id->socket_udp, udp_msgserver->socket_udp);
		}
		else
			maxfd = udp_id->socket_udp;

    	/* Waits until any socket are ready to be read */
		counter = select(maxfd + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, &timeToCheck);


    	/* Occur if there is an error */
		if(counter < 0)
		{
			printf("Error: %s\n", strerror(errno));
			closeAndFree(udp_msgserver, udp_id, params);
			exit(1);
		}
		if(counter == 0) /* Check server availability, if needs to change the message server */
		{
			if(difftime(time(NULL), udp_msgserver->start_time_check.tv_sec) > TOLERANCE_TIME && (udp_msgserver->flag_check_publish == CHECK_PUBLISH || udp_msgserver->flag_check_show_msg == CHECK_SHOW_LATEST_MESSAGES))
			{
				udp_msgserver->flag_available = 0;

				FD_CLR(udp_msgserver->socket_udp, &rfds);
				close(udp_msgserver->socket_udp);
				free(udp_msgserver->serv_ip);


				if(udp_msgserver->flag_check_publish == CHECK_PUBLISH) /* If check publish is active and the time tolerance passed, then message has to be resend to another server */
					udp_msgserver->flag_check_publish = RESEND_MSG;

				if(udp_msgserver->flag_check_show_msg)
					udp_msgserver->flag_check_show_msg = RESEND_MSG;
			}
			if(udp_msgserver->flag_available == 0) /* If tolerance time had been passed, then it has to define another message server randomly */
				defineRandomServer(udp_id);

			timeToCheck.tv_sec = 2;
			timeToCheck.tv_usec = 0;
		}

    	/* Check input from stdin */
		if(FD_ISSET(0, &rfds))
			start(udp_id, udp_msgserver, params); /* Analyzes the input */

    	/* Checks socket set with id server */
		if(FD_ISSET(udp_id->socket_udp, &rfds))
		{
			/* Defines a radom server registered on ID server */
			if(udp_msgserver->flag_available == 0)
				parseDefineServer(udp_id, udp_msgserver, params); /* Defines a new message server */
			else
				parseGetServers(udp_id); /* Parses the response from ID sever */
		}

		if(udp_msgserver->flag_available == 1)
		{
			/* Checks socket set with message server */
			if(FD_ISSET(udp_msgserver->socket_udp, &rfds))
			{
				if(udp_msgserver->flag_check_publish == CHECK_PUBLISH)
					verifyPublish(udp_msgserver, udp_id, params); /* Verifies if message was really published on a message server */
				else if(udp_msgserver->flag_check_show_msg == CHECK_SHOW_LATEST_MESSAGES)
					parseGetMessages(udp_msgserver);	/* Analyzes the response from server with messages required */
			}
		}
	}

}

/*********************************************************************************************************************
 * inicializeRmb()
 *
 * Arguments: udp_id - udp socket established with ID server
 *			  udp_msgserver - udp socket established with a message server
 * Returns: (void)
 * Side-Effects: creates sockets to communicate with ID server and a message server; defines a random message server
 *
 * Description: establishes sockets and defines a message server randomly
 *
 *******************************************************************************************************************/

void inicializeRmb(UDP_Socket *udp_id, Parameters *params)
{
	/* Sets socket UDP for id server */
	establishedUDPsocket(udp_id, ID);

	/* Inicializes messages list that holds the messages that rmb wants to publish */
	params->messages_to_publish = iniList();

	/* Defines a random message server */
	defineRandomServer(udp_id);

}

/************************************************************************************************
 * start()
 *
 * Arguments: udp_id - udp socket established with ID server
 *			  udp_msgserver - udp socket established with a message server
 *			  params - parameters given by user if the option is publish or show_latest_messages
 * Returns: (void)
 * Side-Effects: choose what to do if user gives a command
 *
 * Description: check what command the user typed
 *
 ***********************************************************************************************/

void start(UDP_Socket *udp_id, UDP_Socket *udp_msgserver, Parameters *params)
{
	char option[SIZE_COMMAND];                 /* Holds the comand that the user typed */
 	char buffer[SIZE_MSG+SIZE_COMMAND];        /* Holds the full string that user typed */
	char *second_parameter2=NULL;              /* Holds parameter that user typed */
	

	/* Gets the comand */
  	fgets(buffer, 200, stdin);

	/* Gets command typed by user */
	sscanf(buffer,"%s\n", option);

	/* Gets second_parameter2 if there is any */
	second_parameter2=&buffer[strlen(option)+1];

	/* Shows all servers registered on ID server */
	if(strcmp(option, "show_servers" ) == 0)
		show_servers(udp_id);
	else if(strcmp(option, "publish") == 0 && strlen(second_parameter2) > 1) /* Publishes a message if second_parameter2 is valid */
	{
		if(strlen(second_parameter2) <= SIZE_MSG)
		{
			memset(&params->message, 0, sizeof(params->message));
			strcpy(params->message, second_parameter2);
			publish_message(udp_msgserver, params, INPUT);
		}
		else
			printf("Can't publish: a message has a maximum of 140 characters\n");
	}
	else if(strcmp(option, "publish") == 0 && strlen(second_parameter2) <= 1)
		printf("Please insert the message to send\n\n");  
	else if(strcmp(option, "show_latest_messages") == 0 && strlen(second_parameter2) > 1) /* Shows the latest n messages published (second_parameter2 has to be valid) */
	{
	    /* Converts second parameter to an int */
		params->numberMessages = atoi(second_parameter2);

		if(params->numberMessages > 0)
			get_messages(udp_msgserver, params);
		else
			printf("Insert a valid number\n");
	}
	else if(strcmp(option, "exit") == 0) /* Exits application */
	{
		closeAndFree(udp_msgserver, udp_id, params);
		exit(0);
	}
	else if(strcmp(option, "show_latest_messages") == 0 && strlen(second_parameter2) <= 1)
		printf("Please insert the number of messages to show\n\n");
	else
		printf("\nInvalid command, please insert a valid option!\n\n");

}

/**********************************************************************************
 * show_servers()
 *
 * Arguments: udp_id - udp socket established with ID server
 * Returns: (void)
 * Side-Effects: show all message servers registered on the ID server
 *
 * Description: allows user to know which servers are resgitered on the ID server
 *
 *********************************************************************************/

void show_servers(UDP_Socket *udp_id)
{
	sendToServer(udp_id, "GET_SERVERS");
}

/*******************************************************************************************************************
 * publish_message()
 *
 * Arguments: udp_msgserver - udp socket established with a message server
 *			  params - parameters given by user if the option is publish or show_latest_messages
 * Returns: (void)
 * Side-Effects: publish a message on a message server registered on ID server, randomly choosen by program
 *
 * Description: rmb server publish a message on a message server
 *
 ******************************************************************************************************************/

void publish_message(UDP_Socket *udp_msgserver, Parameters *params, int descriptor)
{
	char msg[SIZE_MSG + 8];
	Messages *new_message;

	t_lista *aux;
	Messages *auxItem;

	strcpy(msg, "PUBLISH ");

	/* Checks if it was a stdin requirement */
	if(descriptor == INPUT)
	{
		/* Puts the message on the list to publish */
		new_message = (Messages*)malloc(sizeof(Messages)); 
		VerifyMalloc(new_message);
		strcpy(new_message->post, params->message);
		params->messages_to_publish = newEndNode(params->messages_to_publish, (Item)new_message);
	}

	aux = params->messages_to_publish;
	auxItem = getListItem(aux);

  	/* Concatenates messages to a string to be send */
	strcat(msg, auxItem->post);

	if(udp_msgserver->flag_available == 1)
	{
		sendToServer(udp_msgserver, msg);
		udp_msgserver->flag_check_publish = CHECK_PUBLISH; /* Checks if message was received by a message server in order to not lose any message sent */
		sendToServer(udp_msgserver, "GET_MESSAGES 10"); /* Asks server for its latest messages in order to know if the message sent was really published */
		udp_msgserver->start_time_check.tv_sec = time(NULL); /* Count the start time to check if message server received the message */
	}
	else if(udp_msgserver->flag_available == 0)
		printf("No message servers registered, can't publish message right now. Message will be saved and resend!\n");

	
}

/*******************************************************************************************************************************
 * get_messages()
 *
 * Arguments: udp_msgserver - udp socket established with a message server
 *			  params - parameters given by user if the option is publish or show_latest_messages
 * Returns: (void)
 * Side-Effects: get messages from a message server registered on ID server, randomly choosen by program
 *
 * Description: user can obtain the number of messages they want to know, asking one of the registeres servers on the ID server
 *
 *******************************************************************************************************************************/

void get_messages(UDP_Socket *udp_msgserver, Parameters *params)
{
	char msg[30];
	char aux[10];
	strcpy(msg, "GET_MESSAGES ");

  	/* Converts a number with 5 digits to a string */
	snprintf(aux, 5, "%d", params->numberMessages);

	strcat(msg, aux);

	sendToServer(udp_msgserver, msg);

	udp_msgserver->flag_check_show_msg = CHECK_SHOW_LATEST_MESSAGES; /* Checks if message server sent the latest n messages required by rmb */
	udp_msgserver->start_time_check.tv_sec = time(NULL); /* Count the start time to check if message server received all required messages */

}

/**************************************************************************************************************
 * parseGetServers()
 *
 * Arguments: udp_id - udp socket established with ID server
 * Returns: (void)
 * Side-Effects: parse of the message received from message server, with all servers registered on ID server
 *
 * Description: shows all servers registered on ID server
 *
 **************************************************************************************************************/

void parseGetServers(UDP_Socket *udp_id)
{
	char *servers_registered;
	char *aux;
	int j = 0;
	char name[25];
	char ip[25];
	int upt;
	int tpt;


  	/* Receives servers registered from ID server */
	receiveFromServer(udp_id);

  	/* Alocates memory for an auxiliary string */
	servers_registered = (char*)malloc((strlen(udp_id->buffer)+1) * sizeof(char));
	VerifyMalloc(servers_registered);

  	/* Aux string that holds servers_registered initial address */
	aux = servers_registered;

	strcpy(servers_registered, udp_id->buffer);

  	/* Puts the pointer after "SERVERS\n" */
	servers_registered=&servers_registered[8];

	if(strlen(servers_registered) <= 1)
		printf("No servers registered\n\n");

 	/* Iterates through server list and prints it */
 	while(strlen(servers_registered)>0)
 	{
    	/* Receives info until '\n' , words dont absorb ';' */
 		sscanf(servers_registered, "%[^;];%[^;];%d;%d", name, ip, &upt, &tpt);

 		printf("%s %s %d %d\n", name, ip, upt, tpt);

		/* Search fot the next '\n' and move the pointer there */
		for (j=1; servers_registered[j] != '\n' && j < strlen(servers_registered); j++){}

    	/* If we are not at the end move to first character of next line */
		if(j < strlen(servers_registered))
			servers_registered=&servers_registered[j+1];

	}

  	/* Frees servers_registered */
	free(aux);
}

/*****************************************************************************************************
 * parseGetMessages()
 *
 * Arguments: udp_msgserver - udp socket established with a message server
 * Returns: (void)
 * Side-Effects: parse of the messages received from a message server, with all messages that it has
 *
 * Description: shows all messages that message server has
 *
 *****************************************************************************************************/

void parseGetMessages(UDP_Socket *udp_msgserver)
{
	char *received_messages;

	receiveFromServer(udp_msgserver);

	udp_msgserver->flag_check_show_msg = SUCCESS; /* Messages required were been sent by message server, means success */

  	/* Puts the pointer after 'MESSAGES\n' */
	received_messages=&udp_msgserver->buffer[9];

	if(strlen(received_messages) > 0)
		printf("%s\n", received_messages);
	else
		printf("\nThere aren't any messages yet!\n");

}

/*****************************************************************************************************
 * defineRandomServer()
 *
 * Arguments: udp_id - udp socket established with an ID server
 * Returns: (void)
 * Side-Effects: asks ID server for all servers registered on itself
 *
 * Description: know which servers are registered in ID server, to define which will be choosen
 *
 *****************************************************************************************************/

void defineRandomServer(UDP_Socket *udp_id)
{
	sendToServer(udp_id, "GET_SERVERS");
}

/********************************************************************************
 * parseDefineServer()
 *
 * Arguments: udp_id - udp socket established with an ID server
 *			  udp_msgserver - udp socket established with a message server
 * Returns: (void)
 * Side-Effects: defines a server to communicate
 *
 * Description: receives all servers registered and choose one of them randomly
 *
 ********************************************************************************/

void parseDefineServer(UDP_Socket *udp_id, UDP_Socket *udp_msgserver, Parameters *params)
{ 
	char *aux ="";
	int i = 0;
	int j = 0;
	int choosen = 0;
	
	udp_msgserver->serv_port = 0;

	t_lista *aux1 = params->messages_to_publish;

	/* Generates seed */
 	srand(time(NULL));

  	/* Gets all message servers registered on ID server */
	receiveFromServer(udp_id);

  	/* If buffer contains servers, picks one randomly */
	if(strlen(udp_id->buffer) > 11)
	{
		aux = udp_id->buffer;
		aux=&aux[8];

		while(strlen(aux) > 0)
		{
			for(i=0; aux[i] != '\n'; i++){} /* Counts '\n' and therefore the number of servers registered */
			aux=&aux[i+1];

			j++;
		}
		
    	/* Picks one server */
    	if(j>1)
			choosen = rand() % (j-1);
		else
			choosen = 0;

		i = 0;
		j = 0;
		aux=udp_id->buffer;
		aux=&aux[8];
	
		while(j != choosen)
		{
			for(i=0; aux[i] != '\n'; i++){} /* Moves the pointer until the next '\n' */
			aux=&aux[i+1];

			j++;
		}

		udp_msgserver->serv_ip = (char*)malloc(50 * sizeof(char));
   		VerifyMalloc(udp_msgserver->serv_ip);

		/* Fills the information about picked server */
		sscanf(aux, "%*[^;];%[^;];%d;%*d", udp_msgserver->serv_ip, &udp_msgserver->serv_port);

		establishedUDPsocket(udp_msgserver, MSG);
		udp_msgserver->flag_available = 1;

		if(aux1 != NULL)
			publish_message(udp_msgserver, params, RESEND_MSG);
		if(udp_msgserver->flag_check_show_msg == RESEND_MSG)
			get_messages(udp_msgserver, params);
	}
}

/**************************************************************************************************
 * verifyPublish()
 *
 * Arguments: udp_id - udp socket established with an ID server
 *			  udp_msgserver - udp socket established with a message server
 *			  params - parameters given by user if the option is publish or show_latest_messages
 * Returns: (void)
 * Side-Effects: verifies if publish was received by a message server
 *
 * Description: if the message wasn't reached a message server, redifines a new one (randomly)
 *
 *************************************************************************************************/

void verifyPublish(UDP_Socket *udp_msgserver, UDP_Socket *udp_id, Parameters *params)
{
	t_lista *aux = params->messages_to_publish;
	Messages *auxItem = getListItem(aux);

	receiveFromServer(udp_msgserver);

	if(strstr(udp_msgserver->buffer, auxItem->post) != NULL) /* Checks if message published is on the message server list of messages */
	{
		udp_msgserver->flag_check_publish = SUCCESS;
		printf("\nMensagem publicada\n");
		params->messages_to_publish = deleteFirstNode(params->messages_to_publish);

		/* Checks if there are messages in list to send */
		if((aux = params->messages_to_publish) != NULL)
			publish_message(udp_msgserver, params, RESEND_MSG);
	}
	else
	{
		udp_msgserver->flag_available = 0;
		udp_msgserver->flag_check_publish = RESEND_MSG; /* Publish needs to be resend */
	}
}

