/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include "client_support.h"

int32_t main(int32_t argc, uint8_t **argv) 
{
	uint8_t command=0;
    	/* check command line arguments */
    	if (argc != 3) 
	{
       		fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       		exit(0);
    	}
    	hostname = argv[1];
    	portno = atoi(argv[2]);

    	/* socket: create the socket */
   	 sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    	if (sockfd < 0) 
        	error("ERROR opening socket");

    	/* gethostbyname: get the server's DNS entry */
    	server = gethostbyname(hostname);
    	if (server == NULL) 
	{
        	fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        	exit(0);
	}

    	/* build the server's Internet address */
    	bzero((uint8_t *) &serveraddr, sizeof(serveraddr));
    	serveraddr.sin_family = AF_INET;
    	bcopy((uint8_t *)server->h_addr, 
	(uint8_t *)&serveraddr.sin_addr.s_addr, server->h_length);
    	serveraddr.sin_port = htons(portno);
	
    	/* get a message from the user */
    	bzero(buf, BUFSIZE);
	
	while(1)
	{
   		printf("Please enter msg: ");
    		fgets(buf, BUFSIZE, stdin);
		command = command_catch(buf);
		switch(command)
		{
			case get:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d get %s",command,filename);
				break;
			}
			case put:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d put %s",command,filename);
				break;
			}
			case del:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d del %s",command,filename);
				break;
			}
			case ls:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d ls",command);
				break;
			}
			case ex:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d ex",command);
				break;
			}
			default:
			{
				syslog(SYSLOG_PRIORITY,"\nNo Command Caught = %d",command);
			}
		}
    		/* send the message to the server */
    		serverlen = sizeof(serveraddr);
    		n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
   		if (n < 0) 
      		error("ERROR in sendto");
    
    		/* print the server's reply */
    		n = recvfrom(sockfd, buf, BUFSIZE, 0, &serveraddr, &serverlen);
    		if (n < 0) 
      			error("ERROR in recvfrom");
    		printf("Echo from server: %s", buf);
	}
	return 0;
}
