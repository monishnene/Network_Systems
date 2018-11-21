/***********************************************************************
 * proxy.c
 * Network Systems CSCI 5273 Programming Assignment 3
 * Author: Monish Nene
 * Date: 11/09/2018
 * @brief This file has main functions for the proxy
 * Design Proxy that operates between web browser and insecure web servers
***********************************************************************/

#include "proxy_support.h"

//the thread function
void *connection_handler(void *);

/***********************************************************************
 * main()
 * @param Port number to be used for proxy configuration
 * @param Timeout after which a cache page expires
 * @brief This function has browser socket creation, process fork and pthread join
 **********************************************************************/
int main(int argc , char *argv[])
{
    int socket_ptr , client_sock , c , *new_sock;
    uint8_t check=1;
    struct sockaddr_in proxy , client;
    if (argc != 3)
    {
        printf ("\nUsage: <portNo> <timeout>\n");
        exit(1);
    }
    else
    {
                timeout = atoi(argv[2]);
    }
    timer.tv_sec=ALIVE_TIME;
    setsockopt(*new_sock,SOL_SOCKET,SO_RCVTIMEO | SO_REUSEADDR | SO_REUSEPORT, (const char*)&timer,sizeof timer);

    //Create socket
    socket_ptr = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_ptr == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    proxy.sin_family = AF_INET;
    proxy.sin_addr.s_addr = INADDR_ANY;
    proxy.sin_port = htons(atoi(argv[1]));

    //Bind
    if( bind(socket_ptr,(struct sockaddr *)&proxy , sizeof(proxy)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_ptr , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while((client_sock = accept(socket_ptr, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        if(check!=0)
	{
		check=fork();
		if(check!=0)
		{
			continue;
		}
	}
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    }

    if (client_sock < 0)
    {
        return 1;
    }

    return 0;
}


/***********************************************************************
 * connection_handler()
 * @param Browser socket pointer
 * @brief This function has the most important tasks of the proxy
 **********************************************************************/
void *connection_handler(void *socket_ptr)
{
    int32_t browser_socket = *(int32_t*)socket_ptr,web_socket;
    int8_t filename[IP_ADDRESS_SIZE],url[DOMAIN_NAME_SIZE],ip[IP_ADDRESS_SIZE],hostname[DOMAIN_NAME_SIZE];
    struct hostent *host_ptr;
    FILE *fptr;
    time_t real_time;
    struct sockaddr_in proxy;
    int32_t data_bytes=0,error_check=0;
    int8_t buffer[BUFFER_SIZE];
    int8_t browser_input[BUFFER_SIZE];
    int8_t port[DATA_SIZE] = "80";
    int8_t url_encoded[IP_ADDRESS_SIZE];
    int8_t* line=NULL;
    size_t length;
    bzero(buffer, sizeof(buffer));
    bzero(browser_input, sizeof(browser_input));
    while((data_bytes = recv(browser_socket, buffer, sizeof(buffer), 0)))
    {
        strncpy(browser_input, buffer, data_bytes);
        bzero(url, sizeof(DOMAIN_NAME_SIZE));
        if(input_check(buffer,url)<0)
        {
            data_bytes = send(browser_socket,  error400,strlen(error400), 0 );
            shutdown(browser_socket,SHUT_RDWR);
            close(browser_socket);
            break;
        }
        md5_encode(url,url_encoded);
        sscanf(url, "%*[^/]%*c%*c%[^/]", hostname);
        printf("\nHostname: %s", hostname );
        if(blacklist_check(hostname,socket_ptr))
        {//url is in blacklist
            break;
        }
        if(cache_check(url))
        {//data found in cache
            bzero(filename, sizeof(filename));
            sprintf(filename, "./cache/%s", url_encoded);
            fptr = fopen(filename, "r");
            getline(&line, &length, fptr);
            bzero(buffer, sizeof(buffer));
            while((data_bytes = fread(buffer, 1, sizeof(buffer), fptr)))
            {
                send(browser_socket, buffer, data_bytes, 0);
                bzero(buffer, sizeof(buffer));
            }
            fclose(fptr);
            shutdown(browser_socket,SHUT_RDWR);
    		close(browser_socket);
            break;
        }
        else
        {//data not found in cache
            bzero(&proxy,sizeof(proxy));
            if(strchr(hostname, ':'))
            {
                bzero(port, sizeof(port));
                sscanf(hostname, "%[^:]%*c%[^/]", ip, port);
                proxy.sin_addr.s_addr = inet_addr(ip);
            }
            else
            {
                int32_t checkHostPresent = IP_cache_check(hostname, ip);
                if(checkHostPresent==1)
                {
                    printf("\nIP found in Cache");
                    proxy.sin_addr.s_addr = inet_addr(ip);
                }
                else
                {
                    printf("\nIP Not Found in Cache");
                    host_ptr = gethostbyname(hostname);
                    if(IP_validity_check(hostname,socket_ptr)<0)
                    {
                        break;
                    }
                    else
                    {
                        bcopy((int8_t*)host_ptr->h_addr, (int8_t*)&proxy.sin_addr, host_ptr->h_length);
                        bzero(filename, sizeof(filename));
                        sprintf(filename, "Hostname_IP");
                        fptr = fopen(filename, "ab");
                        bzero(buffer, sizeof(buffer));
                        sprintf(buffer, "%s %s\n", hostname, inet_ntoa(proxy.sin_addr));
                        fwrite(buffer, 1, strlen(buffer), fptr);
                        fclose(fptr);
                    }
                }
            }
            proxy.sin_family = AF_INET;
            proxy.sin_port = htons(atoi(port));
            if ((web_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                printf("\nError creating socket");
                continue;
            }
            if((connect(web_socket, (struct sockaddr *)&proxy, sizeof(proxy))) < 0)
            {
                printf("\nError in connecting to socket");
                continue;
            }
            send(web_socket, browser_input, strlen(browser_input), 0);
            bzero(buffer, sizeof(buffer));
            bzero(filename, sizeof(filename));
            sprintf(filename, "cache/%s", url_encoded);
            fptr = fopen(filename, "ab");
            real_time = time(NULL);
            fprintf(fptr, "%lu\n", real_time);
            bzero(buffer, sizeof(buffer));
            while(1)
            {
                data_bytes = recv(web_socket, buffer, sizeof(buffer), 0);
                printf("\nbuffer_filled=%d, buffer_len=%ld ",data_bytes,strlen(buffer));
                send(browser_socket, buffer, data_bytes, 0);
                fwrite(buffer, 1, data_bytes, fptr);
                bzero(buffer, sizeof(buffer));
                if(data_bytes==0)
                {
                	break;
                }
            }
            shutdown(browser_socket,SHUT_RDWR);
            close(browser_socket);
            fclose(fptr);
        }
    }
}
