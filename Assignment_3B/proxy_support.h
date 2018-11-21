#ifndef PROXY_SUPPORT_H_
#define PROXY_SUPPORT_H_
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
#include <fcntl.h>
#include <memory.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <pthread.h>

#define DEBUG
#define SYSLOG_PRIORITY 99
#define PACKET_SIZE 32
#define DATA_SIZE 100
#define NEW_LINE 10
#define EOF_NEW 1
#define ALIVE_TIME 10
#define BUFFER_SIZE (1024)
#define CLIENT_MESSAGE_SIZE (2000)
#define DOMAIN_NAME_SIZE (100)
#define IP_ADDRESS_SIZE (32)
#define PORT 8998
#define HEADER_SIZE 300
#define True 1
#define False 0

static uint8_t get_str[]="GET";
static uint8_t http_str[]="http://";
static uint8_t https_str[]="https://";
static uint8_t v0[]="HTTP/1.0";
static uint8_t v1[]="HTTP/1.1";
static uint8_t error404[]=
"HTTP/1.1 HTTP 404 Not Found Error\r\n"
"Content-Type: text/html; charset = UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<body><center><h1>ERROR 404: Not Found Error</h1><br>\r\n";
static uint8_t error400[]=
"HTTP/1.1 HTTP 400 Bad Request Error\r\n"
"Content-Type: text/html; charset = UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<body><center><h1>ERROR 400: Bad Request Error</h1><br>\r\n";
static uint8_t error403[]=
"HTTP/1.1 HTTP 403 Bad Request Error\r\n"
"Content-Type: text/html; charset = UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<body><center><h1>ERROR 403: Forbidden</h1><br>\r\n";
struct timeval timer;
uint32_t timeout;

int8_t* MD5sum(int8_t *url);
int32_t checkCacheFile(int8_t *url);
int32_t checkCacheHost(int8_t *hostname, int8_t *ip);
int32_t checkForbiddenHost(int8_t  *hostname,void *socket_desc);
int32_t check_valid_input(int8_t* buffer,int8_t* url,void *socket_desc);
int32_t check_valid_ip(int8_t *hostname,struct hostent *host_ptr,void *socket_desc);
int32_t linkPrefetch(int8_t* prefetch_ip, int8_t* filename, int8_t* hostname, int8_t* port );
#endif
