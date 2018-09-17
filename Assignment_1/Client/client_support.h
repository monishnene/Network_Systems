#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024

int32_t sockfd, portno, n;
int32_t serverlen;
struct sockaddr_in serveraddr;
struct hostent *server;
uint8_t *hostname;
uint8_t buf[BUFSIZE];

uint8_t command_catch(uint8_t* input);
void error(uint8_t *msg);
