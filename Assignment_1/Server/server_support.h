#ifndef SERVER_SUPPORT_H_
#define SERVER_SUPPORT_H_
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
#include <syslog.h>

#define SYSLOG_PRIORITY 99
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

int32_t sockfd; /* socket */
int32_t portno; /* port to listen on */
uint32_t partner_len; /* byte size of client's address */
struct sockaddr_in serveraddr; /* server's addr */
struct sockaddr_in partner_addr; /* client addr */
struct hostent *hostp; /* client host info */
uint8_t buf[BUFSIZE]; /* message buf */
uint8_t *hostaddrp; /* dotted decimal host addr string */
int32_t optval; /* flag value for setsockopt */
static uint8_t get_str[]="get";
static uint8_t put_str[]="put";
static uint8_t del_str[]="delete";
static uint8_t ls_str[]="ls";
static uint8_t ex_str[]="exit";
static uint8_t EOF_message[]="End_of_File";
static uint8_t delete_command[]="rm -f ";
uint8_t* final_command;
uint8_t filename[20];
struct timeval timer;

uint8_t command_catch(uint8_t* input);
void error(uint8_t *msg);
void send_to_client(uint8_t* str);
int32_t send_file(uint8_t* fname);
int32_t receive_file(uint8_t* fname);
uint32_t receive_packet(uint8_t* data);

#endif
