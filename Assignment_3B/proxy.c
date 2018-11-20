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
        //puts("Connection accepted");
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
        //perror("accept failed");
        return 1;
    }
    	     
    return 0;
}

/***** Response Function for Client Requests *****/
void *connection_handler(void *socket_desc)
{
    int32_t newsockfd = *(int32_t*)socket_desc;
    int8_t filename[BUFFER_SIZE];
    int32_t sockfd1;
    int8_t method[BUFFER_SIZE];
    int8_t url[BUFFER_SIZE];
    int8_t version[BUFFER_SIZE];
    int8_t ip[128] = "";
    int8_t forbid_ip[128] = "";
    int8_t port[32] = "80";
    int8_t hostname[BUFFER_SIZE];
    struct hostent *server_hp;							// to represent entry in host database
    FILE *fp;
    struct sockaddr_in server;
    int32_t nbytes;
    int8_t buffer[BUFFER_SIZE];
    int8_t req_buffer[BUFFER_SIZE];
    int8_t *url_hash;
    int8_t* line=NULL;
    size_t length;
    int32_t flag = 0;
    int32_t pid2;
    bzero(buffer, sizeof(buffer));
    bzero(req_buffer, sizeof(req_buffer));
    while((nbytes = recv(newsockfd, buffer, sizeof(buffer), 0)))
    {
        //printf("Request Buffer:\n%s\n", buffer);
        strncpy(req_buffer, buffer, nbytes);
        bzero(method, sizeof(method));
        bzero(url, sizeof(method));
        bzero(version, sizeof(version));
        sscanf(buffer, "%s %s %s", method, url, version);
        printf("Method: %s \tURL: %s \tVersion:%s\n", method, url, version );
        url_hash = MD5sum(url); //Calling MD5sum function to get hash value to create filename
        //Check for GET request
        if(strcmp(method, "GET") != 0)
        {
            bzero(buffer, sizeof(buffer));
            sprintf(buffer, error400,strlen(error400));
            printf("Error Buffer\n%s\n", buffer);
            nbytes = send(newsockfd, buffer, strlen(buffer), 0 );
            continue;
        }
        //Check for HTTP request
        else if((strstr(url, "https") != NULL) || (strstr(url, "http") == NULL))
        {
            bzero(buffer, sizeof(buffer));
            sprintf(buffer, error400,strlen(error400));
            printf("Error Buffer\n%s\n", buffer);
            nbytes = send(newsockfd, buffer, strlen(buffer), 0 );
            continue;
        }
        //Check for Valid HTTP Version
        else if((strcmp(version, "HTTP/1.0") != 0) && (strcmp(version, "HTTP/1.1") != 0))
        {
            bzero(buffer, sizeof(buffer));
            sprintf(buffer, error400,strlen(error400));
            printf("Error Buffer\n%s\n", buffer);
            nbytes = send(newsockfd, buffer, strlen(buffer), 0 );
            continue;
        }
        else
        {
            sscanf(url, "%*[^/]%*c%*c%[^/]", hostname);
            printf("Hostname: %s\n", hostname );
            if(checkForbiddenHost(hostname))
            {
                printf("Forbidden webpage->%s\n", hostname);
                nbytes = send(newsockfd, error403,strlen(error403), 0 );
		close(newsockfd);
		continue;
            }
            //Function call to check whether file is present in the cache
            int32_t cacheFilePresent = checkCacheFile(url);
            if(cacheFilePresent == 1)
            {
                printf("\n*****Page found in Cache Socket:%d*****\n", newsockfd );
                FILE *fp;
                bzero(filename, sizeof(filename));
                sprintf(filename, "./cache/%s", url_hash);
                fp = fopen(filename, "r");
                getline(&line, &length, fp);
                //sending file to client if found in cache
                bzero(buffer, sizeof(buffer));
                while((nbytes = fread(buffer, 1, sizeof(buffer), fp)))
                {
                    send(newsockfd, buffer, nbytes, 0);
                    bzero(buffer, sizeof(buffer));
                }
                fclose(fp);
                continue;
            }
            else
            {
                printf("\n*****Page Not found in Cache:%d*****\n", newsockfd);
                //Check for a formatting possibility for hostname
                if(strchr(hostname, ':'))
                {
                    bzero(port, sizeof(port));
                    sscanf(hostname, "%[^:]%*c%[^/]", ip, port);
                    bzero(&server,sizeof(server));               //zero the struct
                    server.sin_family = AF_INET;                 //address family
                    server.sin_port = htons(atoi(port));      //sets port to network byte order
                    server.sin_addr.s_addr = inet_addr(ip); //sets remote IP address
                }
                else
                {
                    //Function call to check for Hostname in cache to save the DNS query
                    int32_t checkHostPresent = checkCacheHost(hostname, ip);
                    if(checkHostPresent==1)
                    {
                        printf("\n*******Host Present in Cache*******\n");
                        bzero(filename, sizeof(filename));
                        sprintf(filename, "./cache/hosts");
                        //fp = fopen(filename, "ab");
                        bzero(&server,sizeof(server));               //zero the struct
                        server.sin_family = AF_INET;                 //address family
                        server.sin_port = htons(atoi(port));      //sets port to network byte order
                        server.sin_addr.s_addr = inet_addr(ip); //sets remote IP address
                    }
                    else
                    {
                        printf("\n*******Host Not Present in Cache*******\n");
                        bzero(&server,sizeof(server));               //zero the struct
                        server.sin_family = AF_INET;                 //address family
                        server.sin_port = htons(atoi(port));      //sets port to network byte order
                        //server.sin_addr.s_addr = inet_addr(hostname); //sets remote IP address
                        server_hp = gethostbyname(hostname);					 // Return information about host in argv[1]
                        bcopy((int8_t*)server_hp->h_addr, (int8_t*)&server.sin_addr, server_hp->h_length);
                        //Check for Valid Server
                        if(server_hp < 0)
                        {
                            bzero(buffer, sizeof(buffer));
                            sprintf(buffer, error404,strlen(error404));
                            printf("Error Buffer\n%s\n", buffer);
                            nbytes = send(newsockfd, buffer, strlen(buffer), 0 );
                            continue;
                        }
                        else
                        {
                            bzero(filename, sizeof(filename));
                            sprintf(filename, "./cache/hosts");
                            fp = fopen(filename, "ab");
                            bzero(buffer, sizeof(buffer));
                            sprintf(buffer, "%s %s\n", hostname, inet_ntoa(server.sin_addr));
                            fwrite(buffer, 1, strlen(buffer), fp);
                            fclose(fp);
                        }
                    }
                }
                /***** Creating the socket to fetch data from the remote server*****/
                if ((sockfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                    printf("Error creating socket at proxy \n");
                    continue;
                }
                //Connecting to remote server
                if((connect(sockfd1, (struct sockaddr *)&server, sizeof(server))) < 0)
                {
                    printf("Error in Connect to the server. \n");
                    continue;
                }
                //Sending Request to Remote Server
                send(sockfd1, req_buffer, strlen(req_buffer), 0);
                bzero(buffer, sizeof(buffer));
                bzero(filename, sizeof(filename));
                sprintf(filename, "./cache/%s", url_hash);
                fp = fopen(filename, "ab");
                if(fp < 0)
                {
                    printf("Error Creating Cache file\n");
                    exit(1);
                }
                time_t current_time1;
                current_time1 = time(NULL); //Get Current time
                fprintf(fp, "%lu\n", current_time1); //Appending Current Time to the Cached File
                bzero(buffer, sizeof(buffer));
                while(1)
                {
		    nbytes = recv(sockfd1, buffer, sizeof(buffer), 0);
                    printf("\nbuffer_filled=%d, buffer_len=%ld ",nbytes,strlen(buffer));
                    //Check for Links to be Prefetched
                    if(strstr(buffer, "<html"))
                    {
                        flag = 1; //Flag-Set for a Prefetching Link Found
                    }
                    //Sending the data recieved for the request to the client
                    send(newsockfd, buffer, nbytes, 0);
                    fwrite(buffer, 1, nbytes, fp);
                    bzero(buffer, sizeof(buffer));
		    if(nbytes==0)
		    {
			close(newsockfd);
			exit(0);
		    }	
                }
                fclose(fp);
		//Fork if Prefetch Link Found
                if(flag==1)
                {
                    pid2 = fork();
                    if(pid2 == 0)
                    {
                        int32_t ret = linkPrefetch(inet_ntoa(server.sin_addr), filename, hostname, port );
                        exit(0);
                    }
                }
            }
        }
    }
}
