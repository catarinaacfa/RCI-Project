/***************************************************************************************
 *
 * File Name: servers.c
 *
 * COMMENTS
 *		implements functions to communicate between servers using UDP and TCP protocols
 *
 ***************************************************************************************/

#include "servers1.h"

/********************************************************************************
 * establishedUDPsocket()
 *
 * Arguments: udp - udp session established
 *			  serverUDP - which is the server
 * Returns: (void)
 * Side-Effects: create an udp socket
 *
 * Description: establishes a udp session
 *
 *******************************************************************************/

void establishedUDPsocket(UDP_Socket *udp, int serverUDP)
{
	/* Sets an UDP socket */
	udp->socket_udp = socket(AF_INET,SOCK_DGRAM,0);

	struct hostent *hostptr;

	/* Checks for errors */
	if(udp->socket_udp < 0)
	{
		printf("Error: %s\n", strerror(errno));
		exit(1);
	}

	/* Set up servers, ID and Message server */
	if(serverUDP == ID)
	{
		memset((void*)&(udp->serveraddr), (int)'\0', sizeof(udp->serveraddr));
 		udp->serveraddr.sin_family = AF_INET;

 		if(strcmp(udp->serv_ip, "tejo.tecnico.ulisboa.pt") == 0)
 		{
 			hostptr=gethostbyname(udp->serv_ip);

			if(hostptr==NULL)
			{
				printf("Host not found\n");
				exit(1);
			}

			udp->serveraddr.sin_addr.s_addr = ((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
 		}
 		else
			udp->serveraddr.sin_addr.s_addr = inet_addr(udp->serv_ip);
		
		udp->serveraddr.sin_port= htons((u_short)udp->serv_port);
	}
	else if(serverUDP == MSG)
	{
		memset((void*)&(udp->serveraddr), (int)'\0', sizeof(udp->serveraddr));
		udp->serveraddr.sin_family = AF_INET;
		udp->serveraddr.sin_addr.s_addr = inet_addr(udp->serv_ip); /* IP MSG SERVER */
		udp->serveraddr.sin_port = htons((u_short)udp->serv_port);
	}

}

/*********************************************************************************************************
 * sendToServer()
 *
 * Arguments: udp - udp session established
 *			  msg - message to send
 * Returns: (void)
 * Side-Effects: send a request to the ID server using UDP protocol
 *
 * Description: allows a message server to communicate with the ID server through UDP protocol
 *
 ********************************************************************************************************/

void sendToServer(UDP_Socket *udp, char *msg)
{
	socklen_t addrlen;

	addrlen = sizeof(udp->serveraddr);

	if(sendto(udp->socket_udp, msg, strlen(msg)+1, 0,(struct sockaddr*)&(udp->serveraddr),addrlen)==-1)
	{
		printf("Error: %s\n", strerror(errno));
		exit(1);
	}

}

/************************************************************************************************
 * receiveFromServer()
 *
 * Arguments: udp - udp session established
 * Returns: (void)
 * Side-Effects: ID server answers to the message server request using UDP protocol
 *
 * Description: allows a message server to communicate with the ID server through UDP protocol
 *
 ***********************************************************************************************/

void receiveFromServer(UDP_Socket *udp)
{
	socklen_t addrlen;
	int available_data = 0;

	addrlen = sizeof(udp->serveraddr);
	memset(&udp->buffer, 0, sizeof(udp->buffer)); /* Clean buffer */

	/* Receives and checks if it was all reeived */
	if((available_data = recvfrom(udp->socket_udp, udp->buffer, sizeof(udp->buffer),0,(struct sockaddr*)&(udp->serveraddr),&addrlen)) <= 0)
	{
		if(errno != EINTR && errno != EPIPE)
		{
			printf("Error: %s\n", strerror(errno));
			exit(1);
		}
		if(available_data == 0) /* Message server unavailable */
		{
			sendToServer(udp, "GET_SERVERS");
			udp->flag_available = 0;
		}
	}
	else if(available_data > 0)
	{
		/* If there isn't enough space on buffer throws an error */
		if(available_data > sizeof(udp->buffer))
		{
			printf("UDP packet is bigger than expected: unable to receive all data\n");
			exit(EXIT_FAILURE);
		}
	}
}

/*****************************************************************************
 * closeSockets()
 *
 * Arguments: udp_msgserver - UDP socket established with message server
 *			  udp_id - UDP socket established with ID server
 * Returns: (void)
 * Side-Effects: close sockets
 *
 * Description: close all sockets established through the program
 *
 ****************************************************************************/

void closeSockets(UDP_Socket *udp_msgserver, UDP_Socket *udp_id)
{
	if(udp_id->socket_udp > 0)
		close(udp_id->socket_udp);
	if(udp_msgserver->socket_udp > 0)
		close(udp_msgserver->socket_udp);
}

/***********************************************************************************
 * freeAllMemory()
 *
 * Arguments: udp_msgserver - UDP socket established with message server
 *			  udp_id - UDP socket established with ID server
 *            params - parameters given by user
 * Return:  (void)
 * Side-Effects: frees all memory allocated
 *
 * Description: free memory allocated through the program
 *
 **********************************************************************************/

void freeAllMemory(UDP_Socket *udp_id, Parameters *params)
{
	free(params);
	free(udp_id->serv_ip);
    free(udp_id);
}

/*****************************************************************************
 * closeAndfFree()
 *
 * Arguments: udp_msgserver - UDP socket established with message server
 *			  udp_id - UDP socket established with ID server
 *			  params - parameters given by user
 * Returns: (void)
 * Side-Effects: closes all sockets and frees all memory
 *
 * Description: close sockets and frees memory
 *
 ****************************************************************************/

void closeAndFree(UDP_Socket *udp_msgserver, UDP_Socket *udp_id, Parameters *params)
{
	closeSockets(udp_msgserver, udp_id);
	freeList(params->messages_to_publish, itemFree);
	freeAllMemory(udp_id, params);
	free(udp_msgserver->serv_ip);
	free(udp_msgserver);
}

