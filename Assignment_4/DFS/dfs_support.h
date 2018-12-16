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
#include <time.h>

#define SYSLOG_PRIORITY 99
#define PACKET_SIZE 32
#define DATA_SIZE 100
#define CLIENT_MESSAGE_SIZE 50
#define NEW_LINE 10
#define EOF_NEW 1
#define TIMEOUT 10
#define BUFFER_SIZE (1024*1024*8)
#define PORT 8998
#define HEADER_SIZE 300
#define True 1
#define False 0

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

uint32_t buffer_filled;
int32_t sockfd; /* socket */
int32_t portno; /* port to listen on */
uint32_t partner_len; /* byte size of client's address */
uint8_t *hostaddrp; /* dotted decimal host addr string */
int32_t optval; /* flag value for setsockopt */
static uint8_t get_str[]="GET";
static uint8_t put_str[]="PUT";
static uint8_t ls_str[]="LIST";
static uint8_t server_id=0;
uint8_t buffer[BUFFER_SIZE];
static uint8_t test_str[]="\nWelcome to Monish Nene's Web Server";
static uint8_t conf_filename[]="dfs.conf";
int8_t username[20];
int8_t password[20];
uint8_t filename[20];
uint8_t path[50];
commands method;
struct timeval timer;

short_signals authorization_check(int8_t *username, int8_t *password);
uint8_t command_catch(uint8_t* input);
void error(uint8_t *msg);
int32_t send_file(uint8_t* fname,uint8_t* buffer,uint8_t* postdata);
uint8_t search_str(uint8_t* haystack,uint8_t* needle);
uint8_t file_extension_check(uint8_t* fname, uint8_t* extension);
#endif
