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
