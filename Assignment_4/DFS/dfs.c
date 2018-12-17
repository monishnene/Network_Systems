/***********************************************************************
 * tcp_server.c
 * Network Systems CSCI 5273 Programming Assignment 2
 * Author: Monish Nene
 * Date: 10/09/2018
 * @brief This file has main functions for the server
 * Application file transfer using TCP protocol
***********************************************************************/
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include "dfs_support.h"

//the thread function
void *connection_handler(void *);

int main(int argc , char *argv[])
{
   	int socket_desc , client_sock , c , *new_sock;
    	uint8_t check=1;
    	struct sockaddr_in server , client;
	bzero(path,PATH_SIZE);
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
        	puts("Connection accepted");
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
        	setsockopt(*new_sock,SOL_SOCKET,SO_RCVTIMEO | SO_REUSEADDR | SO_REUSEPORT, (const char*)&timer,sizeof timer);
        	if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        	{
            	perror("could not create thread");
            	return 1;
        	}

        	//Now join the thread , so that we dont terminate before the thread
        	//pthread_join( sniffer_thread , NULL);
        	puts("Handler assigned");
    	}

    	if (client_sock < 0)
    	{
        	perror("accept failed");
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
   	sock = *(int*)socket_desc;
    	int32_t read_size,n=0;
    	uint8_t command=0,response=0;
    	int8_t subfolder[20];
	int8_t mkdir_str[20];
    	uint8_t client_message[CLIENT_MESSAGE_SIZE],server_response[CLIENT_MESSAGE_SIZE];
    	commands command_caught;
    	read_size = recv(sock , client_message , CLIENT_MESSAGE_SIZE , 0);
    	//printf("\nclient to server = %s",client_message);
    	sscanf(client_message, "%hhd %s %s %s",&server_id,username,password,subfolder);
    	//printf("\nServer %d received username:%s password:%s from client\n",server_id,username,password);
	sprintf(path,"DFS%d/%s",server_id,username);
	printf("path=%s",path);
	sprintf(mkdir_str,"mkdir %s",path);
	system(mkdir_str);
	sprintf(path,"DFS%d/%s/%s/",server_id,username,subfolder);
	printf("path=%s",path);
	sprintf(mkdir_str,"mkdir %s",path);
	system(mkdir_str);
    	if((username==NULL)||(password==NULL))
    	{
		response = user_not_found;
		send(sock, &response, 1, 0);
    	}
    	else
    	{
		response=authorization_check(username,password);
    		send(sock, &response, 1, 0);
    	}
    	//Receive a message from client
    	while(1)
    	{	
		bzero(client_message , CLIENT_MESSAGE_SIZE);
		read_size = read(sock,client_message,CLIENT_MESSAGE_SIZE);
		if(read_size<=0)
		{
			break;
		}
		command_caught=command_catch(client_message);
		//if(command_caught > 0)
		{		
			printf("\nInput:%s Command Caught = %d",client_message,command_caught);
			act_server(command_caught);
		} 		
		bzero(client_message , CLIENT_MESSAGE_SIZE);
		printf("\nServer %d ready for next command",server_id);
    	}
    	puts("\nClient disconnected\n");
    	fflush(stdout);
    	shutdown(sock,SHUT_RDWR);
    	close(sock);
    	//Free the socket pointer
    	free(socket_desc);
    	//free(buffer);
    	return 0;
}
