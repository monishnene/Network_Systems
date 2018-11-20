/*
    C socket server example, handles multiple clients using threads
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include "proxy_support.h"
 
//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    uint8_t check=1;
    struct sockaddr_in server , client;
     	
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
	timer.tv_sec=TIMEOUT;
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        //puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        //perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    socket_browser = *(int*)socket_desc;
    int read_size=0,n=0,buffer_filled=0;
    uint8_t command=0,i=0,error_check=0;
    uint8_t* message;
    uint8_t* buffer=(uint8_t*) malloc(BUFFER_SIZE);
    uint8_t* client_message=(uint8_t*) malloc(CLIENT_MESSAGE_SIZE);	
    uint8_t* domain_name=(uint8_t*) malloc(DOMAIN_NAME_SIZE);
    //Receive a message from client
    bzero(buffer, BUFFER_SIZE);
    bzero(client_message, CLIENT_MESSAGE_SIZE);
    bzero(domain_name, DOMAIN_NAME_SIZE);
    while((read_size = recv(socket_browser , client_message , CLIENT_MESSAGE_SIZE , 0)) > 0)
    {
		buffer_filled=0;
        	//Send the message back to client		
		printf("%s",client_message);
		error_check = Domain_Name_Extract(client_message,domain_name);
		buffer_filled = proxy_client(domain_name, client_message);
		if(buffer_filled==-404)
		{
			n=send(socket_browser,error404,strlen(error404),0);
			shutdown(socket_browser,SHUT_RDWR);
    			close(socket_browser); 
			break;
		}
		else
		{
			n=send(socket_browser,error400,strlen(error400),0);
			shutdown(socket_browser,SHUT_RDWR);
    			close(socket_browser); 
			break;
		}
 		bzero(client_message ,  CLIENT_MESSAGE_SIZE);		
    }	
    puts("\nProxy disconnected\n");
    fflush(stdout);
    shutdown(socket_browser,SHUT_RDWR);
    close(socket_browser); 
    //Free the socket pointer
    free(socket_desc);
    free(buffer);
    free(client_message);
    free(domain_name);
    return 0;
}
