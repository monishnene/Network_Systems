#include "client_support.h"
/* 
 * error - wrapper for perror
 */
void error(uint8_t *msg) 
{
	perror(msg);
    	exit(0);
}

uint32_t receive_packet(uint8_t* data)
{
	uint32_t received = 0;
	while(!received)
	{
		received = recvfrom(sockfd, data, PACKET_SIZE, 0, (struct sockaddr *) &partner_addr, partner_len);
	}
	return received;		
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

void read_file(uint8_t* fname)
{
	uint8_t n;
	uint8_t* data=(uint8_t*)malloc(PACKET_SIZE);
	int32_t eof_check=0;
	FILE *fptr;
	if((fptr = fopen(fname,"r")))
	{
		while(eof_check != EOF_NEW)
		{		
			data=fgets(data,PACKET_SIZE,fptr);
			if(data!=NULL)
			{	
				printf("%s",data);
			}
			eof_check=feof(fptr);
		}	
	}
	fclose(fptr);
	return;
}



int32_t send_file(uint8_t* fname)
{
	uint8_t acknowledge=0,package_counter=0;
	int32_t eof_check=0;
	uint8_t n=0,ack=0,i=0;
	uint8_t data[PACKET_SIZE];
	uint8_t tagged_data[sizeof(package_counter)+PACKET_SIZE];
	syslog(SYSLOG_PRIORITY,"send file %s",fname);
	FILE *fptr;
	if(!access(fname,F_OK))
	{	
		fptr = fopen(fname,"r");
		while(eof_check != EOF_NEW)
		{	
			bzero(data, PACKET_SIZE);
			fgets(data,PACKET_SIZE,fptr);
			eof_check=feof(fptr);
			if(fptr!=NULL)
			{		
				if(data!=NULL)
				{	
					ack=0;
					while(!ack)
					{				
						//syslog(SYSLOG_PRIORITY,"package_counter=%ld",package_counter);
						tagged_data[0]=package_counter;
						for(i=0;i<PACKET_SIZE;i++)
						{
							tagged_data[i+1]=data[i];
						}
						n = sendto(sockfd,tagged_data, PACKET_SIZE+ sizeof(package_counter), 0, (struct sockaddr *) &partner_addr, partner_len);
						//syslog(SYSLOG_PRIORITY,"%s",data);
						n = recvfrom(sockfd, &acknowledge, sizeof(acknowledge), 0, (struct sockaddr *)&partner_addr, partner_len);
						//syslog(SYSLOG_PRIORITY,"acknowledge=%ld",acknowledge);
						if(acknowledge==package_counter)
						{
							package_counter++;
							ack=1;						
						}
					}	
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
	n = sendto(sockfd, EOF_message, PACKET_SIZE, 0, (struct sockaddr *) &partner_addr, partner_len);
	return eof_check;
}


int32_t receive_file(uint8_t* fname)
{
	uint8_t acknowledge=0,package_counter=0;
	int32_t error_check=0;
	uint8_t condition=1,n=0,i=0,file_exists=0;
	uint8_t data[PACKET_SIZE];
	uint8_t tagged_data[PACKET_SIZE+sizeof(acknowledge)];	
	syslog(SYSLOG_PRIORITY,"receive file %s",fname);
	FILE *fptr = fopen(fname,"w");
	while(condition)
	{
		//syslog(SYSLOG_PRIORITY,"package_counter=%ld",package_counter);	
		n = recvfrom(sockfd, tagged_data, PACKET_SIZE+sizeof(acknowledge), 0, (struct sockaddr *) &partner_addr, partner_len);
		acknowledge=tagged_data[0];
		for(i=0;i<PACKET_SIZE;i++)
		{
			data[i]=tagged_data[i+1];
		}
		//syslog(SYSLOG_PRIORITY,"acknowledge=%ld",acknowledge);		
		if(!strcmp(data,EOF_message))
		{
			condition=0;
			package_counter--;
			n = sendto(sockfd,&acknowledge, sizeof(acknowledge), 0, (struct sockaddr *) &partner_addr, partner_len);	
		}
		else if(acknowledge==package_counter)
		{
			error_check=fputs(data,fptr);
			n = sendto(sockfd,&acknowledge, sizeof(acknowledge), 0, (struct sockaddr *) &partner_addr, partner_len);
			//syslog(SYSLOG_PRIORITY,"%s",data);
			package_counter++;
			file_exists=1;
		}
		else
		{
			condition=0;
		}
	}
	fclose(fptr);	
	if(!file_exists)
	{
		remove(fname);
	}
	syslog(SYSLOG_PRIORITY,"File Saved %s",fname);
	return package_counter;
}
