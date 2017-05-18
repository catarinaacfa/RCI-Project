/***************************************************************************************
 *
 * File Name: servers1.c
 *
 * COMMENTS
 *		implements functions to communicate between servers using UDP and TCP protocols
 *
 ***************************************************************************************/

#include "servers.h"

/********************************************************************************************************************************
 * establishedUDPsocket()
 *
 * Arguments: udp - udp socket established
 *			  msg_server - message server
 *			  function - checks is it's a client or a server
 * Returns: (void)
 * Side-Effects: establishes an udp socket and fill struct sockaddr_in of ID server or fill struct sockaddr_in of message server 
 *
 * Description: establishes a udp connection
 *
 ********************************************************************************************************************************/

void establishedUDPsocket(UDP_Socket *udp, Server *msg_server, int function)
{
	udp->socket_udp = socket(AF_INET,SOCK_DGRAM,0);

	struct hostent *hostptr;

	if(udp->socket_udp < 0)
	{
		printf("Error: %s\n", strerror(errno));
		exit(1);
	}

	if(function == SERVER)
	{
		memset((void*)&(msg_server->serveraddr_msg_server_udp), (int)'\0', sizeof(msg_server->serveraddr_msg_server_udp));
		msg_server->serveraddr_msg_server_udp.sin_family= AF_INET;
		msg_server->serveraddr_msg_server_udp.sin_addr.s_addr= htonl(INADDR_ANY);
		msg_server->serveraddr_msg_server_udp.sin_port= htons((u_short)msg_server->upt);

		bind(udp->socket_udp,(struct sockaddr*)&(msg_server->serveraddr_msg_server_udp), sizeof(msg_server->serveraddr_msg_server_udp));
	}
	else if(function == CLIENT)
	{
		/* Inicialize struct sockaddr_in of ID server */
		memset((void*)&(msg_server->serveraddr_id), (int)'\0', sizeof(msg_server->serveraddr_id));
 		msg_server->serveraddr_id.sin_family= AF_INET;
 		if(strcmp(msg_server->siip, "tejo.tecnico.ulisboa.pt") == 0)
 		{
 			hostptr=gethostbyname(msg_server->siip);

			if(hostptr==NULL)
			{
				printf("Host not found\n");
				exit(1);
			}

			msg_server->serveraddr_id.sin_addr.s_addr = ((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
 		}
 		else
			msg_server->serveraddr_id.sin_addr.s_addr = inet_addr(msg_server->siip);
		
		msg_server->serveraddr_id.sin_port= htons((u_short)msg_server->sipt);
	}

}

/*********************************************************************************
 * receiveFromRmb()
 *
 * Arguments: rmb - udp socket established with rmb
 *            msg_server - message server
 * Returns: (void)
 * Side-Effects: allows the message server to receive messages from a rmb server
 *
 * Description: allows communication between rmb and message server
 *
 ********************************************************************************/

void receiveFromRmb(UDP_Socket *rmb, Server *msg_server)
{
	socklen_t addrlen;

	addrlen = sizeof(msg_server->client_rmb);
	memset(&rmb->buffer, 0, sizeof(rmb->buffer)); /* Clean buffer */

	if(recvfrom(rmb->socket_udp, rmb->buffer, sizeof(rmb->buffer),0,(struct sockaddr*)&(msg_server->client_rmb), &addrlen)==-1)
	{
		if(errno != EINTR && errno != ECONNRESET && errno != EPIPE)
		{
			printf("Error: %s\n", strerror(errno));
			exit(1);
		}
	}
	
}

/*********************************************************************************
 * sendToRmb()
 *
 * Arguments: rmb - udp socket established with rmb
 *            msg - answer given by message server to rmb server
 *			  msg_server - message server
 * Side-Effects: allows the message server to send messages to a rmb server
 *
 * Description: allows communication between rmb and message server
 *
 ********************************************************************************/

void sendToRmb(UDP_Socket *rmb, Server *msg_server, char *msg)
{
	socklen_t addrlen;

	addrlen = sizeof(msg_server->client_rmb);

	if(sendto(rmb->socket_udp, msg, (strlen(msg)+1), 0,(struct sockaddr*)&(msg_server->client_rmb),addrlen)==-1)
	{
		if(errno != EINTR && errno != EPIPE)
		{
			printf("Error: %s\n", strerror(errno));
			exit(1);
		}
	}
}

/************************************************************************************************
 * sendToIdServer()
 *
 * Arguments: msg_server - message server
 *		 	  udp - udp socket established
 *			  msg - string to send
 * Returns: (void)
 * Side-Effects: send a request to the ID server using UDP protocol
 *
 * Description: allows a message server to communicate with the ID server through UDP protocol
 *
 ***********************************************************************************************/

void sendToIdServer(Server *msg_server, UDP_Socket *udp, char *msg)
{
	socklen_t addrlen;

	addrlen = sizeof(msg_server->serveraddr_id);

	if(sendto(udp->socket_udp, msg, strlen(msg)+1, 0,(struct sockaddr*)&(msg_server->serveraddr_id),addrlen)==-1)
	{
		if(errno != EINTR && errno != EPIPE)
		{
			printf("Error: %s\n", strerror(errno));
			exit(1);
		}
	}
}

/************************************************************************************************
 * receiveFromIdServerr()
 *
 * Arguments: msg_server - message server
 *		 	  udp - udp socket established
 * Returns: (void)
 * Side-Effects: ID server answers to the message server request using UDP protocol
 *
 * Description: allows a message server to communicate with the ID server through UDP protocol
 *
 ***********************************************************************************************/

void receiveFromIdServer(Server *msg_server, UDP_Socket *udp)
{
	socklen_t addrlen;

	addrlen = sizeof(msg_server->serveraddr_id);
	memset(&udp->buffer, 0, sizeof(udp->buffer)); /* Clean buffer */

	if(recvfrom(udp->socket_udp, udp->buffer, sizeof(udp->buffer), 0,(struct sockaddr*)&(msg_server->serveraddr_id),&addrlen) < 0)
	{
		if(errno != EINTR && errno != EPIPE && errno != ECONNRESET)
		{
			printf("Error: %s\n", strerror(errno));
			exit(1);
		}
	}
}

/*********************************************************************************************************************************
 * connectToServersTCP()
 *
 * Arguments: msg_serv - pointer to the message server
 *			  auxItem - used to associate each server the respective socket
 *			  rmb - UDP socket establish with a terminal rmb
 *			  udp_id - UDP socket establish with ID server
 *			  maxfd - max file descriptor
 *			  rfds - file descriptors list
 * Returns: (void)
 * Side-Effects: Estabilsh a TCP session with a server
 *
 * Description: allows a client to establish a TCP session with a server
 *
 *********************************************************************************************************************************/

void connectToServersTCP(Server *msg_server, TCP_Session *auxItem, UDP_Socket *rmb, UDP_Socket *udp_id, int *maxfd, fd_set *rfds)
{
	socklen_t addrlen, lon;
	int status = 0;
	struct timeval tv;
	fd_set connect_set;
	int optval = 0;
	long arg;

	auxItem->socket_id = socket(AF_INET,SOCK_STREAM,0);

	if(auxItem->socket_id<0)
	{
		printf("Error: %s\n", strerror(errno));
		exit(1);
	}

	memset((void*)&(auxItem->serveraddr),(int)'\0',sizeof(auxItem->serveraddr));

	auxItem->serveraddr.sin_family = AF_INET;
	auxItem->serveraddr.sin_addr.s_addr = inet_addr(auxItem->ip);
	auxItem->serveraddr.sin_port = htons((u_short)auxItem->tcp_port);
	addrlen = sizeof(auxItem->serveraddr);

	/* Set non-blocking */
	arg = fcntl(auxItem->socket_id, F_GETFL, NULL); 
  	arg |= O_NONBLOCK; 
  	fcntl(auxItem->socket_id, F_SETFL, arg);

	/* Connects with other message servers */
	status = connect(auxItem->socket_id, (struct sockaddr*)&(auxItem->serveraddr), addrlen);

	if(status < 0)
	{
		if(errno == EINPROGRESS)
		{
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			FD_ZERO(&connect_set);
			FD_SET(auxItem->socket_id, &connect_set);

			status = select(auxItem->socket_id+1, NULL, &connect_set, NULL, &tv);

			if(status < 0 && errno != EINTR)
			{
				fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
				closeAndFree(rmb, udp_id, msg_server);
             	exit(0);
			}
			else if(status>0)
			{
           		lon = sizeof(int); 
          		getsockopt(auxItem->socket_id, SOL_SOCKET, SO_ERROR, (void*)(&optval), &lon); 
			}
			else
		    	eliminateNodeFromConnectedServers(msg_server, auxItem); /* Closes the socket and eliminates that server from the list */
        }
        else if(errno != EINTR && errno != EPIPE)
		    eliminateNodeFromConnectedServers(msg_server, auxItem); /* Closes the socket and eliminates that server from the list */
	}
	else
	{
		/* Set block again */
		arg = fcntl(auxItem->socket_id, F_GETFL, NULL); 
  		arg &= (~O_NONBLOCK); 
  		fcntl(auxItem->socket_id, F_SETFL, arg);

		/* If connects, put the socket in rfds e checks if the maxfd changed */
		FD_SET(auxItem->socket_id, rfds);
		*maxfd = max(*maxfd, auxItem->socket_id);
	}
}

/***************************************************************************************************************************
 * socketTcpMessageServer()
 *
 * Arguments: msg_serv - pointer to the message server
 * Returns: (void)
 * Side-Effects: Estabilshes a TCP socket for message server and makes it available to listen for outcoming message servers
 *
 * Description: allows clients to connect with message server
 *
 ***************************************************************************************************************************/

void socketTcpMessageServer(Server *msg_server)
{
	msg_server->socket_tcp = socket(AF_INET,SOCK_STREAM,0);
	int optval = 1;

	if(msg_server->socket_tcp < 0)
	{
		printf("Error: %s\n", strerror(errno));
		exit(1);
	}

	memset((void*)&(msg_server->serveraddr_msg_server_tcp),(int)'\0',sizeof(msg_server->serveraddr_msg_server_tcp));
	msg_server->serveraddr_msg_server_tcp.sin_family = AF_INET;
	msg_server->serveraddr_msg_server_tcp.sin_addr.s_addr = htonl(INADDR_ANY);
	msg_server->serveraddr_msg_server_tcp.sin_port = htons((u_short)msg_server->tpt);

	/* Let us rerun the server immediatly after we exit the program, it means that we eliminate the "ERROR on binding: Address already in use" error */
  	if(setsockopt(msg_server->socket_tcp, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(optval)) == -1)
  	{
  		printf("Error: %s\n", strerror(errno));
		exit(1);
  	}
	
	if(bind(msg_server->socket_tcp,(struct sockaddr*)&(msg_server->serveraddr_msg_server_tcp), sizeof(msg_server->serveraddr_msg_server_tcp))==-1)
	{
		printf("Error: %s\n", strerror(errno));
		exit(1);
	}

	if(listen(msg_server->socket_tcp, 5) == -1)
	{
		printf("Error: %s\n", strerror(errno));
		exit(1);
	}

}

/*************************************************************************************************************
 * \ptNewClientsTCP()
 *
 * Arguments: msg_serv - pointer to the message server
 *			  new_client - new client that wants to establish a TCP session
 * Returns: (void)
 * Side-Effects: accept clients
 *
 * Description: message server accepts to establish session with a client
 *
 *************************************************************************************************************/

int acceptNewClientsTCP(Server *msg_server, TCP_Session *new_client)
{
	socklen_t clientlen;
	clientlen = sizeof(new_client->clientaddr);

	if((new_client->socket_id = accept(msg_server->socket_tcp, (struct sockaddr*)&(new_client->clientaddr), &clientlen)) < 0)
	{
		if(errno == ECONNABORTED) /* Checks if connection has been aborted */
			printf("Connection has been aborted\n");
		if(errno == EPERM) /* Firewall rules forbid connection */
			printf("Can't accept connection due to firewall rules\n");
		else if(errno != EINTR && errno != EPIPE)
		{
			printf("Error: %s\n", strerror(errno));
			exit(1);
		}	
	}

	return new_client->socket_id;
}

/************************************************************************************************************************************************
 * readTCP()
 *
 * Arguments: auxTCP - contains the information about the client
 *			  rfds - rfds file descriptor list
 * Returns: (int) nbytes - alerts if message server couldn't read from that socket
 * Side-Effects: 
 *
 * Description: if message server couldn't read through this socket, the connection will be closed and cleared from rfds file descriptor list
 *
 ***********************************************************************************************************************************************/

int readTCP(TCP_Session *auxTCP, fd_set *rfds, Server *msg_server)
{
	int nbytes = 0;

	memset(&msg_server->buffer_read, 0, sizeof(msg_server->buffer_read)); /* Clean buffer */

	if((nbytes = read(auxTCP->socket_id, msg_server->buffer_read, sizeof(msg_server->buffer_read))) <= 0)
	{
		if(nbytes == 0 || errno == EBADF || errno == ECONNRESET)
			printf("Socket connected with IP %s on port %d disconnected\n", auxTCP->ip, auxTCP->tcp_port);
		else if(errno != EINTR && errno != EPIPE)
		{
			printf("Error: %s\n", strerror(errno));
			exit(1);
		}

		/* Remove it from rfds file descriptor */
		FD_CLR(auxTCP->socket_id, rfds);

		/* close the socket */
		close(auxTCP->socket_id); 
		auxTCP->socket_id = -1;
		
	}

	return nbytes;
}

/************************************************************************************************
 * writeTCP()
 *
 * Arguments: auxTCP - contains the information about the client
 * 			  msg - message that will be send by message server to another message server
 *			  msglen - size of message to send
 *			  msg_server - message server
 * Returns: (void)
 * Side-Effects: writes through auxTCP socket
 *
 * Description: allows message server to write messages to another message server
 *
 ***********************************************************************************************/

int writeTCP(TCP_Session *auxTCP, char *msg, unsigned long int msglen, Server *msg_server)
{
	int total = 0;
	int bytes_written = 0;

	while(total < msglen)
	{
		if((bytes_written = write(auxTCP->socket_id, msg, strlen(msg)+1)) == -1)
		{
			if(errno == EPIPE)
			{
				eliminateNodeFromConnectedServers(msg_server, auxTCP);
				break;
			}
			else if(errno != EINTR)
			{
				printf("Error: %s\n", strerror(errno));
				exit(1);
			}
		}
		else
			total += bytes_written;
	}

	return bytes_written;
} 

/**********************************************************************************************************************************
 * eliminateNodeFromConnectedServers()
 *
 * Arguments: auxItem - contains the information about the server we want to eliminate from list after can't estalish connection
 * 			  msg_server - message server
 * Returns: (void)
 * Side-Effects: eliminates a node from connected servers list
 *
 * Description: if a socket is not listening for connections, we can't establish TCP session, so we have to eliminate it from list
 *
 **********************************************************************************************************************************/

void eliminateNodeFromConnectedServers(Server *msg_server, TCP_Session *auxItem)
{
	t_lista *aux;
	TCP_Session *auxTCP = NULL;
	int position_in_list = 1;

	aux = msg_server->connected_servers;
	auxTCP = getListItem(aux);

	/* Search for the right node to delete */
	while(aux != NULL && strcmp(auxTCP->ip, auxItem->ip) != 0)
	{
		aux = getNextListElement(aux);
		if(aux != NULL)
			auxTCP = getListItem(aux);
		position_in_list++;
	}

	close(auxTCP->socket_id);
	
	/* Eliminates the node from the list */
	if(position_in_list == 1)
		msg_server->connected_servers = deleteFirstNode(msg_server->connected_servers, NAME);
	else
		msg_server->connected_servers = deleteMiddleNode(msg_server->connected_servers, position_in_list);
}


/*****************************************************************************
 * closeSockets()
 *
 * Arguments: msg_serv - message server
 *			  rmb - UDP socket established with rmb
 *			  udp_id - UDP socket established with ID server
 * Returns: (void)
 * Side-Effects: close sockets
 *
 * Description: close all sockets established through the program
 *
 ****************************************************************************/

void closeSockets(Server *msg_server, UDP_Socket *rmb, UDP_Socket *udp_id)
{
	t_lista *aux;
	TCP_Session *auxItem = NULL;

	aux = msg_server->connected_servers;
	
    if(aux != NULL)
    	auxItem = getListItem(aux);

	while(aux != NULL)
	{
    	if(auxItem->socket_id > 0)
        	close(auxItem->socket_id);
        
		aux = getNextListElement(aux);
        
        if(aux != NULL)
       		auxItem = getListItem(aux);  
	}

	close(udp_id->socket_udp);
	close(rmb->socket_udp);

	if(msg_server->socket_tcp > 0)
		close(msg_server->socket_tcp);

}

/***********************************************************************************
 * freeAllMemory()
 *
 * Arguments: msg_server - message server
 *            rmb - UDP socket established with rmb
 *            udp_id - UDP socket established with ID server
 * Return:  (void)
 * Side-Effects: frees all memory allocated
 *
 * Description: free memory allocated through the program
 *
 **********************************************************************************/

void freeAllMemory(Server *msg_server, UDP_Socket *rmb, UDP_Socket *udp_id)
{   
    free(udp_id);
    free(rmb);
    
    if(msg_server->name != NULL)
    	free(msg_server->name);
    if(msg_server->siip != NULL)
    	free(msg_server->siip);
    freeList(msg_server->ordered_messages, freeItemWithMessage);
    freeList(msg_server->connected_servers, freeItemWithName);
    free(msg_server);
}

/***********************************************************************************
 * closeAndFree()
 *
 * Arguments: msg_server - message server
 *            rmb - UDP socket established with rmb
 *            udp_id - UDP socket established with ID server
 * Return:  (void)
 * Side-Effects: frees all memory allocated and closes all sockets
 *
 * Description: free memory allocated through the program and close sockets 
 *
 **********************************************************************************/

void closeAndFree(UDP_Socket *rmb, UDP_Socket *udp_id, Server *msg_server)
{
	closeSockets(msg_server, rmb, udp_id);
	freeAllMemory(msg_server, rmb, udp_id);
}

/*****************************************************************************************************************
 * freeAndCloseIni()
 *
 * Arguments: msg_server - message server
 *            rmb - UDP socket established with rmb
 *            udp_id - UDP socket established with ID server
 * Return:  (void)
 * Side-Effects: frees all memory allocated and closes all sockets if an error ocurred on read arguments function
 *
 * Description: frees memory allocated and closes sockets on read arguments function
 *
 ****************************************************************************************************************/

void freeAndCloseIni(UDP_Socket *rmb, UDP_Socket *udp_id, Server *msg_server)
{
	close(udp_id->socket_udp);
	close(rmb->socket_udp);

	if(msg_server->name != NULL)
    	free(msg_server->name);

    if(msg_server->siip != NULL)
    	free(msg_server->siip);

    free(msg_server);
}
