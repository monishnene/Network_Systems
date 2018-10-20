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
#define PACKET_SIZE 32
#define DATA_SIZE 100
#define NEW_LINE 10
#define EOF_NEW 1
#define TIMEOUT_BIG 1000000
#define TIMEOUT_SMALL 10
#define BUFFER_SIZE (1024*1024*8)
#define PORT 8998
#define HEADER_SIZE 200

typedef enum
{
	GET=1,
	HEAD,
	POST,
}commands;

uint32_t buffer_filled;
int32_t sockfd; /* socket */
int32_t portno; /* port to listen on */
uint32_t partner_len; /* byte size of client's address */
struct sockaddr_in serveraddr; /* server's addr */
struct sockaddr_in partner_addr; /* client addr */
struct hostent *hostp; /* client host info */
uint8_t *hostaddrp; /* dotted decimal host addr string */
int32_t optval; /* flag value for setsockopt */
static uint8_t get_str[]="GET";
static uint8_t head_str[]="HEAD";
static uint8_t post_str[]="POST";
static uint8_t test_str[]="\nWelcome to Monish Nene's Web Server";
static uint8_t error500[]="HTTP/1.1 500 Internal Server Error";
static uint8_t html_str[]="html";
static uint8_t plain_str[]="plain";
static uint8_t txt_str[]="txt";
static uint8_t png_str[]="png";
static uint8_t gif_str[]="gif";
static uint8_t jpg_str[]="jpg";
static uint8_t css_str[]="css";
static uint8_t js_str[]="js";
static uint8_t java_str[]="javascript";
struct timeval timer;

uint8_t command_catch(uint8_t* input,uint8_t* buffer);
void error(uint8_t *msg);
int32_t send_file(uint8_t* fname,uint8_t* buffer);

#endif
