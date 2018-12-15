/***********************************************************************
 * dfc.c
 * Network Systems CSCI 5273 Programming Assignment 4
 * Author: Monish Nene
 * Date: 12/09/2018
 * @brief This file has the main loop for the DFC
 * Application Distributed file system
***********************************************************************/
#include "dfc_support.h"


/***********************************************************************
 * @brief main
 * This funtion has the initialization and main loop with switch case for udp client
 * @param argc
 * @param argv localhost and portno
***********************************************************************/
int32_t main(int32_t argc, uint8_t **argv)
{
	if (argc != 2)
    	{
        	printf ("\nUsage: <filename.conf>\n");
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
	int32_t web_socket[TOTAL_SERVERS],read_size=0;
	uint8_t i=0,authorization[TOTAL_SERVERS],authorization_check=0;
	for(i=0;i<TOTAL_SERVERS;i++)
	{
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
            		printf("\nError in connecting to socket");
            		continue;
        	}
		sprintf(configuration_str,"%d %s %s",i+1,username,password);
        	send(web_socket[i], configuration_str, strlen(configuration_str), 0);
		read_size = recv(web_socket[i], &authorization[i] , 1, 0);
	}
	fclose(fptr);
	for(i=0;i<TOTAL_SERVERS;i++)
	{
		authorization_check+=authorization[i];
	}
	printf("\nauthorization_check = %d",authorization_check);
	if(authorization_check==TOTAL_SERVERS*approved)
	{		
		printf("\nAuthorization Successful %d",authorization_check);
		while(1)
		{
			i++;
		}
	}
	else if(authorization_check==TOTAL_SERVERS*user_not_found)
	{
		printf("\nUser ID didn't match %d",authorization_check);
	}
	else if(authorization_check==TOTAL_SERVERS*incorrect_password)
	{
		printf("\nPassword didn't match %d",authorization_check);
	}
	else
	{
		printf("\nUnidentified authorization failure %d",authorization_check);
	}
}
