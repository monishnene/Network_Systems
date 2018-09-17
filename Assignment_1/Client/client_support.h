#ifndef CLIENT_SUPPORT_H_
#define CLIENT_SUPPORT_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <syslog.h>

#define SYSLOG_PRIORITY 100
#define BUFSIZE 1024

typedef enum
{
	get=1,
	put,
	del,
	ls,
	ex,
}commands;

int32_t sockfd, portno, n;
int32_t serverlen;
struct sockaddr_in serveraddr;
struct hostent *server;
uint8_t *hostname;
uint8_t buf[BUFSIZE];
static uint8_t get_str[]="get";
static uint8_t put_str[]="put";
static uint8_t del_str[]="delete";
static uint8_t ls_str[]="ls";
static uint8_t ex_str[]="exit";
uint8_t filename[20];

void error(uint8_t *msg);
uint8_t command_catch(uint8_t* input);

#endif
