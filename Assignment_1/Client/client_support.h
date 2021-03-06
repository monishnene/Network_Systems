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
#define PACKET_SIZE 32
#define DATA_SIZE 100
#define NEW_LINE 10
#define EOF_NEW 1
#define TIMEOUT_BIG 1000000
#define TIMEOUT_SMALL 10

typedef enum
{
	get=1,
	put,
	del,
	ls,
	ex,
}commands;

int32_t sockfd, portno;
uint32_t partner_len;
struct sockaddr_in partner_addr;
struct hostent *server;
uint8_t *hostname;
uint8_t buf[BUFSIZE];
static uint8_t get_str[]="get";
static uint8_t put_str[]="put";
static uint8_t del_str[]="delete";
static uint8_t ls_str[]="ls";
static uint8_t ex_str[]="exit";
static uint8_t EOF_message[]="End_of_File";
uint8_t filename[20];
struct timeval timer;

void error(uint8_t *msg);
uint8_t command_catch(uint8_t* input);
int32_t send_file(uint8_t* fname);
int32_t receive_file(uint8_t* fname);
void read_file(uint8_t* fname);
uint32_t receive_packet(uint8_t* data);

#endif
