/*
    C socket server example, handles multiple clients using threads
*/

#include "proxy_support.h"

//the thread function

//the thread function
void *connection_handler(void *);

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    uint8_t check=1;
    struct sockaddr_in server , client;
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
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
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

void *connection_handler(void *socket_desc)
{
    int32_t newsockfd = *(int32_t*)socket_desc,sockfd1;
    int8_t filename[BUFFER_SIZE],url[DOMAIN_NAME_SIZE],ip[IP_ADDRESS_SIZE],hostname[DOMAIN_NAME_SIZE];
    struct hostent *host_ptr;
    FILE *fp;
    time_t current_time;
    struct sockaddr_in server;
    int32_t nbytes=0,error_check=0;
    int8_t buffer[BUFFER_SIZE];
    int8_t req_buffer[BUFFER_SIZE];
    int8_t port[DATA_SIZE] = "80";
    int8_t url_encoded[IP_ADDRESS_SIZE];
    int8_t* line=NULL;
    size_t length;
    int32_t flag = 0;
    int32_t pid2;
    bzero(buffer, sizeof(buffer));
    bzero(req_buffer, sizeof(req_buffer));
    while((nbytes = recv(newsockfd, buffer, sizeof(buffer), 0)))
    {
        strncpy(req_buffer, buffer, nbytes);
        bzero(url, sizeof(DOMAIN_NAME_SIZE));
	if(check_valid_input(buffer,url)<0)
	{
            nbytes = send(newsockfd,  error400,strlen(error400), 0 );
            shutdown(newsockfd,SHUT_RDWR);
	    close(newsockfd);
	    break;
	}
        md5_encode(url,url_encoded); 
        sscanf(url, "%*[^/]%*c%*c%[^/]", hostname);
        printf("\nHostname: %s", hostname );
        if(checkForbiddenHost(hostname,socket_desc))
        {
		break;
        }
	   if(checkCacheFile(url))
	   {
                bzero(filename, sizeof(filename));
                sprintf(filename, "./cache/%s", url_encoded);
                fp = fopen(filename, "r");
                getline(&line, &length, fp);
                bzero(buffer, sizeof(buffer));
                while((nbytes = fread(buffer, 1, sizeof(buffer), fp)))
                {
                    send(newsockfd, buffer, nbytes, 0);
                    bzero(buffer, sizeof(buffer));
                }
                fclose(fp);
    		shutdown(newsockfd,SHUT_RDWR);
    		close(newsockfd);
                break;
            }
            else
            {
                printf("\nNot found in Cache", newsockfd);
		bzero(&server,sizeof(server));                          
		if(strchr(hostname, ':'))
                {
                    bzero(port, sizeof(port));
                    sscanf(hostname, "%[^:]%*c%[^/]", ip, port);  
                    server.sin_addr.s_addr = inet_addr(ip); 
                }
                else
                {
                    int32_t checkHostPresent = checkCacheHost(hostname, ip);
                    if(checkHostPresent==1)
                    {
                        printf("\nHostname found in Cache");  
                        server.sin_addr.s_addr = inet_addr(ip); 
                    }
                    else
                    {
                        printf("\nHostname Not Found in Cache");
                        host_ptr = gethostbyname(hostname);
                        if(check_valid_ip(hostname,socket_desc)<0)
			{
				break;
			}
                        else
                        {
                            bcopy((int8_t*)host_ptr->h_addr, (int8_t*)&server.sin_addr, host_ptr->h_length);
                            bzero(filename, sizeof(filename));
                            sprintf(filename, "Hostnames_IP");
                            fp = fopen(filename, "ab");
                            bzero(buffer, sizeof(buffer));
                            sprintf(buffer, "%s %s\n", hostname, inet_ntoa(server.sin_addr));
                            fwrite(buffer, 1, strlen(buffer), fp);
                            fclose(fp);
                        }
                    }
                }
                server.sin_family = AF_INET;                 
                server.sin_port = htons(atoi(port));
                if ((sockfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                    printf("\nError creating socket");
                    continue;
                }
                if((connect(sockfd1, (struct sockaddr *)&server, sizeof(server))) < 0)
                {
                    printf("\nError in connecting to socket");
                    continue;
                }
                send(sockfd1, req_buffer, strlen(req_buffer), 0);
                bzero(buffer, sizeof(buffer));
                bzero(filename, sizeof(filename));
                sprintf(filename, "cache/%s", url_encoded);
                fp = fopen(filename, "ab");
                current_time = time(NULL); 
                fprintf(fp, "%lu\n", current_time); 
                bzero(buffer, sizeof(buffer));
                while(1)
                {
		    nbytes = recv(sockfd1, buffer, sizeof(buffer), 0);
                    printf("\nbuffer_filled=%d, buffer_len=%ld ",nbytes,strlen(buffer));
                    send(newsockfd, buffer, nbytes, 0);
                    fwrite(buffer, 1, nbytes, fp);
                    bzero(buffer, sizeof(buffer));
		    if(nbytes==0)
		    {
                	break;
		    }
                }
		shutdown(newsockfd,SHUT_RDWR);
		close(newsockfd);
                fclose(fp);
            }
        }
}
