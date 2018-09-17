/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include "server_support.h"

int32_t main(int32_t argc, uint8_t **argv) 
{
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
  	clientlen = sizeof(clientaddr);
  	while (1) 
	{

    	/*
     	* recvfrom: receive a UDP datagram from a client
     	*/
    	bzero(buf, BUFSIZE);
    	n = recvfrom(sockfd, buf, BUFSIZE, 0,(struct sockaddr *) &clientaddr, &clientlen);
    	if (n < 0)
      		error("ERROR in recvfrom");

    	/* 
     	* gethostbyaddr: determine who sent the datagram
     	*/
    	hostp = gethostbyaddr((const uint8_t *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    	if (hostp == NULL)
      		error("ERROR on gethostbyaddr");
    	hostaddrp = inet_ntoa(clientaddr.sin_addr);
    	if (hostaddrp == NULL)
      		error("ERROR on inet_ntoa\n");
    	printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
   	printf("server received %ld/%d bytes: %s\n", strlen(buf), n, buf);
    
    	/* 
     	* sendto: echo the input back to the client 
     	*/
    	n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &clientaddr, clientlen);
    	if (n < 0) 
     		error("ERROR in sendto");
  }
}
