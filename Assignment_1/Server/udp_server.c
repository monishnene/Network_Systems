/***********************************************************************
 * udp_server.c
 * Network Systems CSCI 5273 Programming Assignment 1
 * Author: Monish Nene
 * Date: 09/09/2018
 * @brief This file has the main loop for the server
 * Application file transfer using UDP protocol
***********************************************************************/

#include "server_support.h"
/***********************************************************************
 * @brief main
 * This funtion has the initialization and main loop with switch case for udp server
 * @param argc
 * @param argv portno
***********************************************************************/
int32_t main(int32_t argc, uint8_t **argv)
{
	uint8_t condition=1,command=0,n=0;
	int32_t error_check=11;
  	/*
   	* check command line arguments
   	*/
  	if (argc != 2)
	{
    		fprintf(stderr, "usage: %s <port>\n", argv[0]);
    		exit(1);
  	}
  	portno = atoi(argv[1]);

  	/*
   	* socket: create the parent socket
   	*/
  	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  	if (sockfd < 0)
  		error("ERROR opening socket");

  	/* setsockopt: Handy debugging trick that lets
  	* us rerun the server immediately after we kill it;
  	* otherwise we have to wait about 20 secs.
   	* Eliminates "ERROR on binding: Address already in use" error.
   	*/
  	optval = 1;
  	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int));

  	/*
   	* build the server's Internet address
   	*/
  	bzero((uint8_t *) &serveraddr, sizeof(serveraddr));
  	serveraddr.sin_family = AF_INET;
  	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	serveraddr.sin_port = htons((unsigned short)portno);

  	/*
   	* bind: associate the parent socket with a port
   	*/
  	if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		error("ERROR on binding");

  	/*
   	* main loop: wait for a datagram, then echo it
   	*/
  	partner_len = sizeof(partner_addr);
  	while (condition)
	{
    		/*
     		* recvfrom: receive a UDP datagram from a client
     		*/
    		bzero(buf, BUFSIZE);
    		n = recvfrom(sockfd, buf, BUFSIZE, 0,(struct sockaddr *) &partner_addr, &partner_len);
    		if (n < 0)
      			error("ERROR in recvfrom");

    		/*
     		* gethostbyaddr: determine who sent the datagram
     		*/
    		hostp = gethostbyaddr((const uint8_t *)&partner_addr.sin_addr.s_addr, sizeof(partner_addr.sin_addr.s_addr), AF_INET);
    		if (hostp == NULL)
      			error("ERROR on gethostbyaddr");
    		hostaddrp = inet_ntoa(partner_addr.sin_addr);
    		if (hostaddrp == NULL)
      			error("ERROR on inet_ntoa\n");
    		printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
   		printf("server received %ld/%d bytes: %s\n", strlen(buf), n, buf);
    		/*SOCKET TIMEOUT*/
		timer.tv_usec = TIMEOUT_BIG;
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&timer,sizeof(timer));
		command = command_catch(buf);
		switch(command)
		{
			case get:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d get %s",command,filename);
				error_check=send_file(filename);
				syslog(SYSLOG_PRIORITY,"errorcheck=%d",error_check);
				if(!error_check)
				{
					send_to_client("File not found.\n");
				}
				else
				{
					send_to_client("File sent.\n");
				}
				break;
			}
			case put:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d put %s",command,filename);
				error_check=receive_file(filename);
				if(!error_check)
				{
					send_to_client("File not received.\n");
				}
				else
				{
					send_to_client("File received.\n");
				}
				break;
			}
			case del:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d del %s",command,filename);
				remove(filename);
				send_to_client("Delete done\n");
				break;
			}
			case ls:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d ls",command);
				system("rm -f ls.txt");
				system("ls>>ls.txt");
				send_file("ls.txt");
				send_to_client("ls done\n");
				break;
			}
			case ex:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d ex",command);
				send_to_client("Server Exit\n");
				condition=0;
				break;
			}
			default:
			{
				syslog(SYSLOG_PRIORITY,"No Command Caught = %d",command);
				strcat(buf,"It is an Invalid Command.\n");
				send_to_client(buf);
				bzero(buf, BUFSIZE);
				break;
			}
		}
    		if (n < 0)
     			error("ERROR in sendto");
  	}
}
