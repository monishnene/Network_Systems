#include "server_support.h"
/* 
 * error - wrapper for perror
 */
void error(uint8_t *msg) 
{
	perror(msg);
    	exit(0);
}

void send_to_client(uint8_t* str)
{
	/* 
     	* sendto: echo the input back to the client 
     	*/
    	n = sendto(sockfd, str, strlen(str), 0, (struct sockaddr *) &clientaddr, clientlen);
	return;
}

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
	else if(!strncmp(input,del_str,strlen(del_str)))
	{
		command_caught=del;
		input += strlen(del_str)+1;
		while(*(input)!=NEW_LINE)
		{
			filename[i++]=*(input++);
		}
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

int32_t send_file(uint8_t* fname)
{
	int32_t eof_check=0;
	uint8_t* data=(uint8_t*)malloc(PACKET_SIZE);
	syslog(SYSLOG_PRIORITY,"send file %s",fname);
	FILE *fptr;
	if(!access(fname,F_OK))
	{	
		fptr = fopen(fname,"r");
		while(eof_check != EOF_new)
		{		
			fgets(data,PACKET_SIZE,fptr);
			eof_check=feof(fptr);
			if(eof_check == EOF_new)
			{
				break;
			}
			if(fptr!=NULL)
			{		
				if(data!=NULL)
				{	
					n = sendto(sockfd, data, PACKET_SIZE, 0, (struct sockaddr *) &clientaddr, clientlen);
					syslog(SYSLOG_PRIORITY,"%s",data);
				}
			}
		}
		fclose(fptr);
	}
	else
	{		
		syslog(SYSLOG_PRIORITY,"The file %s not found",fname);
		eof_check=0;
	}
	n = sendto(sockfd, EOF_message, PACKET_SIZE, 0, (struct sockaddr *) &clientaddr, clientlen);
	return eof_check;
}

int32_t receive_file(uint8_t* fname)
{
	uint64_t acknowlege=0,package_counter=0;
	int32_t error_check=0;
	uint8_t condition=1;
	uint8_t* data=(uint8_t*)malloc(PACKET_SIZE);	
	syslog(SYSLOG_PRIORITY,"receive file %s",fname);
	FILE *fptr = fopen(fname,"w");
	while(condition)
	{
		n = recvfrom(sockfd, data, PACKET_SIZE, 0, (struct sockaddr *) &clientaddr, clientlen);		
		package_counter++;
		if(!strcmp(data,EOF_message))
		{
			package_counter--;
			condition=0;		
		}
		else if(data!=NULL)
		{
			error_check=fputs(data,fptr);
			syslog(SYSLOG_PRIORITY,"%s",data);
		}
		else
		{
			condition=0;
		}
	}
	fclose(fptr);	
	if(!package_counter)
	{
		remove(fname);
	}
	syslog(SYSLOG_PRIORITY,"File Saved %s",fname);
	return package_counter;
}
