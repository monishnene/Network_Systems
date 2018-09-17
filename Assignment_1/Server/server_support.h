#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

typedef enum
{
	get=1,
	put,
	del,
	ls,
	exit,
}commands;

int32_t sockfd; /* socket */
int32_t portno; /* port to listen on */
int32_t clientlen; /* byte size of client's address */
struct sockaddr_in serveraddr; /* server's addr */
struct sockaddr_in clientaddr; /* client addr */
struct hostent *hostp; /* client host info */
uint8_t buf[BUFSIZE]; /* message buf */
uint8_t *hostaddrp; /* dotted decimal host addr string */
int32_t optval; /* flag value for setsockopt */
int32_t n; /* message byte size */
uint8_t get_str[]="get";
uint8_t del_str[]="put";
uint8_t del_str[]="delete";

uint8_t command_catch(uint8_t* input);
void error(uint8_t *msg);
