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

#define DEBUG
#define SYSLOG_PRIORITY 99
#define PACKET_SIZE 32
#define DATA_SIZE 100
#define NEW_LINE 10
#define EOF_NEW 1
#define TIMEOUT 10
#define BUFFER_SIZE (1024)
#define CLIENT_MESSAGE_SIZE (2000)
#define DOMAIN_NAME_SIZE (100)
#define IP_ADDRESS_SIZE (32)
#define PORT 8998
#define HEADER_SIZE 300
#define True 1
#define False 0

#define MAXBUFSIZE 1024
#define ERR_METHOD "HTTP/1.1 400 Bad Request\n\rContent-Type: text/html\nContent-Length: %d\n\r\n<html><body><H1>Error 400 Bad Request: Method Not Supported </H1></body></html>"
#define ERR_URL "HTTP/1.1 400 Bad Request\n\rContent-Type: text/html\nContent-Length: %d\n\r\n<html><body><H1>Error 400 Bad Request: Invalid URL </H1></body></html>"
#define ERR_VERSION "HTTP/1.1 400 Bad Request\n\rContent-Type: text/html\nContent-Length: %d\n\r\n<html><body><H1>Error 400 Bad Request: Invalid HTTP Version </H1></body></html>"
#define ERR_SERVERNOTFOUND "HTTP/1.1 400 Bad Request\n\rContent-Type: text/html\nContent-Length: %d\n\r\n<html><body><H1>Error 400 Bad Request: Server Not Found </H1></body></html>"
#define ERR_BLOCKED "HTTP/1.1 403 Forbidden\n\rContent-Type: text/html\nContent-Length: %d\n\r\n<html><body>ERROR 403 Forbidden</body></html>"

char* MD5sum(char *url);
int checkCacheFile(char *url, unsigned long int timeout);
int checkCacheHost(char *hostname, char *ip);
int checkForbiddenHost(char *hostname, char *forbid_ip);
int linkPrefetch(char* prefetch_ip, char* filename, char* hostname, char* port );
void response(int newsockfd, unsigned long int timeout);
#endif
