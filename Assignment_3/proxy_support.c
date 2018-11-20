/***********************************************************************
 * client_support.c
 * Network Systems CSCI 5273 Programming Assignment 1
 * Author: Monish Nene
 * Date: 09/09/2018
 * @brief This file has supporting functions for the server
 * Application file transfer using UDP protocol
***********************************************************************/
#include "proxy_support.h"

void error(uint8_t *msg)
{
	perror(msg);
    	exit(0);
}

int proxy_client(uint8_t* domain_name, uint8_t* input)
{
	uint8_t ip_found=0,i=0,j=0,n=0,port=80,buffer[1024];
	int sock=0,status=0;
	struct sockaddr_in server;
	struct hostent *host_ptr=gethostbyname(domain_name);
	if(host_ptr!=NULL)
	{
		ip_found=1;
		#ifdef DEBUG
		printf("\nReal Name->%s",host_ptr->h_name);
		while(*(host_ptr->h_aliases+i)!=NULL)
		{
			printf("\nAlias = %s",*(host_ptr->h_addr_list+i++));
		}		
		printf("\nIp found");
		#endif
	}
	else
	{
		printf("\nIP not found");
		status=-404;
		return status;	
	}
	//create socket
        sock = socket(AF_INET, SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("\nProxy Client socket not created.");
		return status;
	}
	printf("\nProxy Client socket created");
	bcopy((uint8_t*)host_ptr->h_addr,(uint8_t*)&server.sin_addr.s_addr,host_ptr->h_length);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if(connect(sock, (struct sockaddr *)&server , sizeof(server))<0)
	{
		perror("\nconnect failed. Error");
		close(sock);	
		return status;
	}
	printf("\nProxy Client Connected");
	//status=send(sock,input, strlen(input) , 0);
	status=write(sock,input, strlen(input));
	if(status<0)
	{
		printf("\nSend failed");	
		close(sock);		
		return status;
	}
		
	bzero(buffer, sizeof(buffer));
        while((status = recv(sock, buffer, sizeof(buffer), 0)))
	{
	  printf("\nbuffer_filled=%d, buffer_len=%ld ",status,strlen(buffer));
          send(socket_browser, buffer, status, 0);
          bzero(buffer, sizeof(buffer));
        }
	if(status < 0)
	{
		printf("\nReceive failed");	
		close(sock);	
		return status;
	}
	else
	{
		printf("\n%d bytes data received from the web server",status);
	}
	status=1;
	//close(sock);
	return status;
}

/***********************************************************************
 * @brief search_str()
 * Search a needle string in another haystack string.
 * @param haystack string to be searched in
 * @param needle small string to be searched
 * @return 1 if ound 0 if not found
***********************************************************************/
uint8_t search_str(uint8_t* haystack,uint8_t* needle)
{
	uint8_t i=0,j=0,match_counter=0,len=0,temp=0;
	len=strlen(needle);
	while(*(haystack+i)!=0)
	{
		if(*(haystack+i++)==*(needle+j++))
		{
			match_counter++;
			temp=i;
			if(match_counter==len)
			{
						return True;
			}
			while(*(needle+j)!=0)
			{
				if(*(haystack+i++)==*(needle+j++))
				{
					match_counter++;
					if(match_counter==len)
					{
						return True;
					}
				}
				else
				{
					i=temp;
					j=0;
					break;				
				}
			}
		}
		else
		{
				j=0;
				match_counter=0;	
		}
	}
	return False;
}

/***********************************************************************
 * @brief command_catch()
 * This funtion is used to convert strings to command.
 * @param data pointer to the input string
 * @return command caught equivalent enum
***********************************************************************/
uint8_t Domain_Name_Extract(uint8_t* input,uint8_t* domain_name)
{
	uint8_t command_caught=0,i=0,j=0,n=0;
	if(search_str(input,get_str))
	{		
		method=GET;
		i=strlen(get_str)+strlen(http_str)+1;
		while((*(input+i)!=32)&&(*(input+i)!='/'))
		{
			*(domain_name+j++)=*(input+i++);
		}
		n=strlen(domain_name);
		if(*(domain_name+n-1)=='/')
		{
			*(domain_name+n-1)=0;	
		}
		command_caught=1;
		#ifdef DEBUG	
		printf("\nDomain Name = %s",domain_name);
		#endif
	}
	return command_caught;
}



