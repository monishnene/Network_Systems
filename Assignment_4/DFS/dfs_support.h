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
	GET,
	HEAD,
	POST,
}commands;

typedef enum
{
	approved=0,
	user_not_found,
	incorrect_password,
	file_not_found,
}short_signals;

uint32_t buffer_filled;
static uint8_t alive=True;
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
static uint8_t server_id=0;
static uint8_t test_str[]="\nWelcome to Monish Nene's Web Server";
static uint8_t conf_filename[]="dfs.conf";
static uint8_t error500[]=
"HTTP/1.1 500 Internal Server Error\r\n"
"Content-Type: text/html; charset = UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<body><center><h1>ERROR 500: Internal Server Error</h1><br>\r\n";
static uint8_t html_str0[]="html";
static uint8_t html_str1[]="text/html";
static uint8_t txt_str1[]="text/plain";
static uint8_t txt_str0[]="txt";
static uint8_t png_str0[]="png";
static uint8_t gif_str0[]="gif";
static uint8_t jpg_str0[]="jpg";
static uint8_t css_str0[]="css";
static uint8_t js_str0[]="js";
static uint8_t alive_str[]="keep-alive";
static uint8_t png_str1[]="image/png";
static uint8_t gif_str1[]="image/gif";
static uint8_t jpg_str1[]="image/jpg";
static uint8_t css_str1[]="text/css";
static uint8_t js_str1[]="application/javascript";
static uint8_t blank_line[]="\r\n\r\n";
int8_t username[20];
int8_t password[20];
commands method;
struct timeval timer;

short_signals authorization_check(int8_t *username, int8_t *password);
uint8_t command_catch(uint8_t* input,uint8_t* buffer);
void error(uint8_t *msg);
int32_t send_file(uint8_t* fname,uint8_t* buffer,uint8_t* postdata);
uint8_t search_str(uint8_t* haystack,uint8_t* needle);
uint8_t file_extension_check(uint8_t* fname, uint8_t* extension);
#endif
