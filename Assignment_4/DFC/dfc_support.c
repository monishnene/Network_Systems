/***********************************************************************
 * client_support.c
 * Network Systems CSCI 5273 Programming Assignment 1
 * Author: Monish Nene
 * Date: 09/09/2018
 * @brief This file has supporting functions for the client
 * Application file transfer using UDP protocol
***********************************************************************/
#include "dfc_support.h"
/**
 * error - wrapper for perror
 */
void error(uint8_t *msg)
{
	perror(msg);
    	exit(0);
}

void remove_newline_char(int8_t* str)
{
	while(*(str)!=0)
	{
		if(*(str)=='\n')
		{
			*(str)=0;		
			return;		
		}
		str++;
	}
	return;
}

/***********************************************************************
 * @brief command_catch()
 * This funtion is used to convert strings to command.
 * @param data pointer to the input string
 * @return command caught equivalent enum
***********************************************************************/
uint8_t command_catch(uint8_t* input)
{
	uint8_t command_caught=0,i=0;
	if(!strncmp(input,get_str,strlen(get_str)))
	{
		command_caught=get;
		input += strlen(get_str)+1;
		while(*(input)!=NEW_LINE)
		{
			filename[i++]=*(input++);
		}
	}
	else if(!strncmp(input,put_str,strlen(put_str)))
	{
		command_caught=put;
		input += strlen(put_str)+1;
		while(*(input)!=NEW_LINE)
		{
			filename[i++]=*(input++);
		}
	}
	else if(!strncmp(input,ls_str,strlen(ls_str)))
	{
		command_caught=list;
	}
	return command_caught;
}

uint8_t act_client(commands command)
{
	uint8_t error_check=0;
	switch(command)
	{
		case get:
		{
			receive_file(filename);
			break;
		}
		case put:
		{
			error_check=send_file(filename);
			if(!error_check)
			{
				printf("File %s is not found.\n",filename);
			}
			break;
		}
		default:
		{
			syslog(SYSLOG_PRIORITY,"\nNo Command Caught = %d",command);
		}
	}
}

uint8_t receive_file(filename,uint8_t server_ID)
{
	
}

uint8_t send_file(filename,uint8_t server_ID)
{
	
}

