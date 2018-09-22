#include "client_support.h"
/* 
 * error - wrapper for perror
 */
void error(uint8_t *msg) 
{
	perror(msg);
    	exit(0);
}

uint8_t command_catch(uint8_t* input)
{
	uint8_t command_caught=0;
	if(!strncmp(input,get_str,strlen(get_str)))
	{
		command_caught=get;
		input += strlen(get_str)+1;
		strcpy(filename,input);
	}
	else if(!strncmp(input,put_str,strlen(put_str)))
	{
		command_caught=put;
		input += strlen(put_str)+1;
		strcpy(filename,input);
	}
	else if(!strncmp(input,del_str,strlen(del_str)))
	{
		command_caught=del;
		input += strlen(del_str)+1;
		strcpy(filename,input);
	}
	else if(!strncmp(input,ls_str,strlen(ls_str)))
	{
		command_caught=ls;
	}
	else if(!strncmp(input,ex_str,strlen(ex_str)))
	{
		command_caught=ex;
	}
	return command_caught;
}

void send_file(uint8_t* fname)
{
	int32_t eof_check=0;
	uint8_t* data=(uint8_t*)malloc(PACKET_SIZE);
	FILE *fptr = fopen(fname,"r");
	while(eof_check != EOF_new)
	{		
		data=fgets(data,PACKET_SIZE,fptr);
		if(data!=NULL)
		{	
			n = sendto(sockfd, data, PACKET_SIZE, 0, (struct sockaddr *) &serveraddr, serverlen);
		}
		eof_check=feof(fptr);
	}
	n = sendto(sockfd, EOF_message, PACKET_SIZE, 0, (struct sockaddr *) &serveraddr, serverlen);
	fclose(fptr);
	return;
}

void receive_file(uint8_t* fname)
{
	uint32_t error_check=0;
	uint8_t* data=(uint8_t*)malloc(PACKET_SIZE);
	uint8_t condition=1;
	FILE *fptr = fopen(fname,"w");
	while(condition)
	{
		n = recvfrom(sockfd, data, PACKET_SIZE, 0, (struct sockaddr *) &serveraddr, serverlen);
		if(!strcmp(data,EOF_message))
		{
			condition=0;
			break;		
		}
		if(data!=NULL)
		{
			error_check=fputs(data,fptr);
		}
		else
		{
			break;
		}
	}	
	fclose(fptr);
	return;
}
