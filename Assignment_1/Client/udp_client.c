/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include "client_support.h"

int32_t main(int32_t argc, uint8_t **argv) 
{
	uint8_t command=0,n;
	int32_t error_check=11;
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
    	bzero((uint8_t *) &partner_addr, sizeof(partner_addr));
    	partner_addr.sin_family = AF_INET;
    	bcopy((uint8_t *)server->h_addr, 
	(uint8_t *)&partner_addr.sin_addr.s_addr, server->h_length);
    	partner_addr.sin_port = htons(portno);
	
    	/* get a message from the user */
    	bzero(buf, BUFSIZE);
	
	while(1)
	{
		/*SOCKET TIMEOUT*/
		timer.tv_usec = TIMEOUT_BIG;
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&timer,sizeof(timer));
		bzero(buf, BUFSIZE);
   		printf("Please enter msg: ");
    		fgets(buf, BUFSIZE, stdin);
		command = command_catch(buf);
		/* send the message to the server */
    		partner_len = sizeof(partner_addr);
    		n = sendto(sockfd, buf, strlen(buf), 0, &partner_addr, partner_len);
   		if (n < 0) 
      		error("ERROR in sendto");
		switch(command)
		{
			case get:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d get %s",command,filename);
				receive_file(filename);
				break;
			}
			case put:
			{
				syslog(SYSLOG_PRIORITY,"\nCommand Caught = %d put %s",command,filename);
				error_check=send_file(filename);
				syslog(SYSLOG_PRIORITY,"errorcheck=%d",error_check);
				if(!error_check)
				{				
					syslog(SYSLOG_PRIORITY,"File %s is not found.",filename);
					printf("File %s is not found.\n",filename);
				}				
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
				receive_file("ls.txt");
				read_file("ls.txt");
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
    		/* print the server's reply */
		bzero(buf, BUFSIZE);
    		n = receive_packet(buf);
    		if (n < 0) 
      			error("ERROR in recvfrom");
    		printf("Server: %s", buf);
	}
	return 0;
}
