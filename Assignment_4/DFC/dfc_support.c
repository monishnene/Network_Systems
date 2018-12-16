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
        bzero(filename,20);
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
	else if(!strncmp(input,mkdir_str,strlen(mkdir_str)))
	{
		command_caught=mkdir;
		input += strlen(mkdir_str)+1;
		while(*(input)!=NEW_LINE)
		{
			filename[i++]=*(input++);
		}
	}
	return command_caught;
}

int32_t split_file()
{
	int32_t data_bytes=0,n=0,eof_check=0,file_size=0,split[5];
	uint8_t temp_filename[20],i=0;
	FILE* fptr=fopen(filename, "r");
	FILE* split_fptr;
	fseek(fptr,0,SEEK_END);
	file_size=ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	bzero(buffer,BUFFER_SIZE);
	n=fread(buffer,1,file_size,fptr);
	split[0]=0;
	split[1]=file_size/4;
	split[2]=file_size/2;
	split[3]=(file_size*3)/4;
	split[4]=file_size;
	printf("\nsplit info = %d %d %d %d %d %d",n,split[0],split[1],split[2],split[3],split[4]);
	if(n==file_size)
	{
		for(i=0;i<TOTAL_SERVERS;i++)
		{
			bzero(temp_filename,20);
			sprintf(temp_filename,"%s.%d",filename,i+1);
			split_fptr = fopen(temp_filename, "w");
			printf("\nnew split filename = %s",temp_filename);
			fwrite(&buffer[split[i]], 1, split[i+1]-split[i], split_fptr);
			fclose(split_fptr);
		}
	}
	fclose(fptr);
	return file_size;
}

int32_t merge_file()
{
	int32_t data_bytes=0,n=0,eof_check=0,file_size=0,split[5];
	uint8_t temp_filename[20],i=0;
	FILE* fptr=fopen("merge_test", "w");
	FILE* split_fptr;
	for(i=0;i<TOTAL_SERVERS;i++)
	{	
		bzero(buffer,BUFFER_SIZE);
		bzero(temp_filename,20);
		sprintf(temp_filename,"%s.%d",filename,i+1);
		split_fptr = fopen(temp_filename, "r");
		fseek(split_fptr,0,SEEK_END);
		file_size=ftell(split_fptr);
		fseek(split_fptr,0,SEEK_SET);
		n=fread(buffer,1,file_size,split_fptr);
		printf("\nmerge filename = %s",temp_filename);
		fwrite(buffer, 1, file_size,fptr);
		fclose(split_fptr);
	}	
	fseek(fptr,0,SEEK_END);
	file_size=ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	fclose(fptr);
	return file_size;
}

uint8_t act_client(commands command)
{
	int32_t error_check=0,n=0;
	uint8_t i=0;
	switch(command)
	{
		case get:
		{
			for(i=0;i<TOTAL_SERVERS;i++)
			{
				//error_check=receive_file(i);
			}
			break;
		}
		case put:
		{
			error_check=split_file();
			n=merge_file();
			if(error_check==n)
			{
				printf("\nSplit and merge filesize matches");
			}
			for(i=0;i<TOTAL_SERVERS;i++)
			{
				error_check=send_file(i);
			}			
			if(!error_check)
			{
				printf("\nFile %s is not found.\n",filename);
			}
			break;
		}
		case mkdir:
		{
			break;
		}
		case list:
		{
			break;
		}
		default:
		{
			printf("\nInvalid Command");
		}
	}
	return error_check;
}

uint8_t send_file(uint8_t server_ID)
{	
	int32_t data_bytes=0,n=0,eof_check=0,file_size=0;
	uint8_t data[PACKET_SIZE],i=0,receiver_ready=0;
	FILE* fptr=fopen(filename, "r");
	fseek(fptr,0,SEEK_END);
	file_size=ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	n = read(web_socket[server_ID],&receiver_ready,sizeof(receiver_ready));
	write(web_socket[server_ID],&file_size,sizeof(file_size));
	bzero(buffer,BUFFER_SIZE);
	n=fread(buffer,1,file_size,fptr);
	if(n==file_size)
	{
		printf("\nFile %s with %d bytes sent to servers",filename,file_size);	
		write(web_socket[server_ID],buffer,file_size);
	}
	else
	{
		printf("\nFile size error file_size = %d, n = %d",file_size,n);
	}
	fclose(fptr);
	return file_size;
}

