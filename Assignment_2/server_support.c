/***********************************************************************
 * client_support.c
 * Network Systems CSCI 5273 Programming Assignment 1
 * Author: Monish Nene
 * Date: 09/09/2018
 * @brief This file has supporting functions for the server
 * Application file transfer using UDP protocol
***********************************************************************/
#include "server_support.h"
/**
 * error - wrapper for perror
 */
void error(uint8_t *msg)
{
	perror(msg);
    	exit(0);
}


/***********************************************************************
 * @brief command_catch()
 * This funtion is used to convert strings to command.
 * @param data pointer to the input string
 * @return command caught equivalent enum
***********************************************************************/
uint8_t command_catch(uint8_t* input,uint8_t* buffer)
{
	uint8_t command_caught=0,i=4,j=3,n;
	uint8_t filepath[100];
	bzero(filepath, 100);
	if(!strncmp(input,get_str,strlen(get_str)))
	{
		if(*(input+5)==32)
		{
			send_file("www/index.html",buffer);
		}
		else
		{
			strcpy(filepath,"www");
			while(*(input+i)!=32)
			{
				filepath[j++]=*(input+i++);
			}
			send_file(filepath,buffer);
		}	
	}
	else if(!strncmp(input,head_str,strlen(head_str)))
	{
		/*command_caught=put;
		input += strlen(put_str)+1;
		while(*(input)!=NEW_LINE)
		{
			filename[i++]=*(input++);
		}*/
	}
	else if(!strncmp(input,post_str,strlen(post_str)))
	{
		/*command_caught=del;
		input += strlen(del_str)+1;
		while(*(input)!=NEW_LINE)
		{
			filename[i++]=*(input++);
		}*/
	}
	return command_caught;
}

/***********************************************************************
 * @brief send_file()
 * This funtion is used to send a file to client via udp protocol
 * @param fname filename of the file to be sent
 * @return eof_check whether file exist or not
***********************************************************************/
int32_t send_file(uint8_t* fname,uint8_t* buffer)
{
	uint8_t acknowledge=0,package_counter=0;
	int32_t file_size=0,eof_check=0;
	uint8_t n=0,ack=0,i=0;
	uint8_t data[PACKET_SIZE];
	uint8_t header[HEADER_SIZE];
	uint8_t* file_type;
	FILE *fptr;
	if(!access(fname,F_OK))
	{
		//file_size
		fptr = fopen(fname,"r");
		fseek(fptr,0,SEEK_END); 
		file_size=ftell(fptr);		
		fseek(fptr,0,SEEK_SET);
		file_type=html_str;
		sprintf(header,"HTTP/1.1 200 Ok\r\n Content-Type:%s\r\nContent-Length:%d\r\n\r\n",file_type,file_size);		
		memcpy(buffer,header,strlen(header));
		buffer_filled+=strlen(header);
		buffer+=strlen(header);
		while(eof_check != EOF_NEW)
		{
			bzero(data, PACKET_SIZE);
			//fgets(data,PACKET_SIZE,fptr);
			n=fread(data,1,PACKET_SIZE,fptr);
			memcpy(buffer,data,PACKET_SIZE);
			buffer+=PACKET_SIZE;
			buffer_filled+=PACKET_SIZE;
			eof_check=feof(fptr);
		}
		fclose(fptr);
	}
	else
	{
		printf("%s file not found",fname);
		//syslog(SYSLOG_PRIORITY,"The file %s not found",fname);
		eof_check=send_file("www/index.html",buffer);
	}
	return eof_check;
}

