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

#define TOTAL_SERVERS 4
#define SYSLOG_PRIORITY 100
#define PACKET_SIZE 32
#define DATA_SIZE 100
#define CLIENT_MESSAGE_SIZE 50
#define NEW_LINE 10
#define EOF_NEW 1
#define TIMEOUT_BIG 1000000
#define TIMEOUT_SMALL 10
#define BUFFER_SIZE (1024*1024*8)

typedef enum
{
	get=1,
	put,
	list,
}commands;

typedef enum
{
	approved=0,
	user_not_found,
	incorrect_password,
	file_not_found,
}short_signals;

int32_t sockfd, portno,web_socket[TOTAL_SERVERS];
uint32_t partner_len;
struct sockaddr_in partner_addr;
struct hostent *server;
uint8_t *hostname;
uint8_t buffer[BUFFER_SIZE];
static uint8_t get_str[]="GET";
static uint8_t put_str[]="PUT";
static uint8_t ls_str[]="LIST";
static uint8_t EOF_message[]="End_of_File";
uint8_t configuration_str[CLIENT_MESSAGE_SIZE];
uint8_t configuration_response[10];
uint8_t filename[20];
uint8_t path[50];
static uint8_t server_id=0;
struct timeval timer;

void remove_newline_char(int8_t* str);
void error(uint8_t *msg);
uint8_t command_catch(uint8_t* input);
int32_t send_file(uint8_t* fname);
int32_t receive_file(uint8_t* fname);
void read_file(uint8_t* fname);
uint32_t receive_packet(uint8_t* data);

#endif
