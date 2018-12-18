/***********************************************************************
 * dfc.c
 * Network Systems CSCI 5273 Programming Assignment 4
 * Author: Monish Nene
 * Date: 12/09/2018
 * @brief This file has the main loop for the DFS Client
 * Application Distributed file system
***********************************************************************/
#include "dfc_support.h"


/***********************************************************************
 * @brief main
 * This funtion has the initialization and main loop with switch case for udp client
 * @param argc
 * @param argv conf file and sub folder
***********************************************************************/
int32_t main(int32_t argc, uint8_t **argv)
{
	if (argc != 3)
    	{
        	printf ("\nUsage: <filename.conf> <subfolder>\n");
        	exit(1);
    	}
	FILE* fptr;
	size_t length;
	int8_t* username=NULL;
	int8_t* password=NULL;
	int8_t* port_no=NULL;
	if((fptr = fopen(argv[1], "r")) == NULL)
    	{
		printf ("\n%s conf file not found\n",argv[2]);
        	exit(1);
	}
        if(getline(&username, &length, fptr) == -1)
       	{
		printf ("\nusername not found\n");
        	exit(1);
	}
        if(getline(&password, &length, fptr) == -1)
       	{
		printf ("\npassword not found\n");
        	exit(1);
	}
	remove_newline_char(username);
	remove_newline_char(password);
	struct sockaddr_in server[TOTAL_SERVERS];
	int32_t read_size=0,n=0;
	uint8_t i=0,authorization[TOTAL_SERVERS],authorization_check=0;
	uint8_t client_input[CLIENT_MESSAGE_SIZE],server_response[CLIENT_MESSAGE_SIZE];
	timer.tv_sec=TIMEOUT;
	commands command_caught;
	bzero(server_status_on,TOTAL_SERVERS);
	for(i=0;i<TOTAL_SERVERS;i++)
	{
		setsockopt(web_socket[i],SOL_SOCKET,SO_RCVTIMEO, (const char*)&timer,sizeof timer);
		server[i].sin_addr.s_addr = INADDR_ANY;
        	server[i].sin_family = AF_INET;
		if(getline(&port_no, &length, fptr) == -1)
       		{
			printf ("\n port for server %d not found\n",++i);
        		exit(1);
		}
        	server[i].sin_port = htons(atoi(port_no));
		if ((web_socket[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        	{
            		printf("\nError creating socket");
            		continue;
        	}
        	if((connect(web_socket[i], (struct sockaddr *)&server[i], sizeof(server[i]))) < 0)
        	{
            		printf("\nServer %d with port no. %d not alive.",i+1,atoi(port_no));
            		continue;
        	}
		sprintf(configuration_str,"%d %s %s %s",i+1,username,password,argv[2]);
        	send(web_socket[i], configuration_str, strlen(configuration_str), 0);
		read_size = read(web_socket[i], &authorization[i] , 1);
		if(authorization[i]==approved)
		{
			server_status_on[i]=1;
			printf("\nAuthorization Successful for Server %d port no. %d",i+1,atoi(port_no));
		}
		else if(authorization_check==user_not_found)
		{
			printf("\nUser ID didn't match for server %d",i+1);
		}
		else if(authorization_check==incorrect_password)
		{
			printf("\nPassword didn't match for server %d",i+1);
		}
		else if(authorization_check==file_not_found)
		{
			printf("\nserver %d conf file_not_found,",i+1);
		}
		else
		{
			printf("\nUnidentified authorization failure for server %d",i+1);
		}
	}
	fclose(fptr);
	strcpy(key,password);
	for(i=0;i<TOTAL_SERVERS;i++)
	{
		authorization_check+=server_status_on[i];
	}
	if(authorization_check>0)
	{
		while(1)
		{
			bzero(client_input, CLIENT_MESSAGE_SIZE);
			printf("\nPlease enter msg: ");
    			fgets(client_input, CLIENT_MESSAGE_SIZE, stdin);
			command_caught=command_catch(client_input);
			printf("\nInput:%s Command Caught = %d, filename = %s",client_input,command_caught,filename);
			for(i=0;i<TOTAL_SERVERS;i++)
			{
				if(server_status_on[i])
				{
					write(web_socket[i], client_input, strlen(client_input));
				}
			}
			act_client(command_caught);
		}
	}
	else
	{
		printf("\nNo server is authorized so switching OFF the client.\n");
	}
}
