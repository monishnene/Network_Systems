/***********************************************************************
 * client_support.c
 * Network Systems CSCI 5273 Programming Assignment 1
 * Author: Monish Nene
 * Date: 09/09/2018
 * @brief This file has supporting functions for the server
 * Application file transfer using UDP protocol
***********************************************************************/
#include "proxy_support.h"
/**
 * error - wrapper for perror
 */
void error(uint8_t *msg)
{
	perror(msg);
    	exit(0);
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
		while(*(input+i)!=32)
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

uint8_t Get_IP(uint8_t** ip_addr_list,uint8_t* domain_name)
{
	uint8_t ip_found=0,i=0,j=0,n=0;
	struct hostent *host_ptr=gethostbyname(domain_name);
	struct in_addr **temp_list;
	if(host_ptr!=NULL)
	{
		ip_found=1;
		temp_list= (struct in_addr **)host_ptr->h_addr_list;
		#ifdef DEBUG
		printf("\nReal Name->%s",host_ptr->h_name);
		while(*(host_ptr->h_aliases+i)!=NULL)
		{
			printf("\nAlias = %s",*(host_ptr->h_addr_list+i++));
		}
		#endif
		for(i=0;host_ptr->h_addr_list[i]!=NULL;i++)
		{
			#ifdef DEBUG
			printf("\nIP=%s",inet_ntoa(*temp_list[i]));
			#endif			
			strcpy(ip_addr_list[i],inet_ntoa(*temp_list[i]));
		}
	}
	#ifdef DEBUG	
	if(ip_found)
	{
		printf("\nIp found");
	}	
	#endif
	return ip_found;
}

/***********************************************************************
 * @brief file_extension_check()
 * This funtion is used to check file extension
 * @param fname filename of the file 
 * @param extension to be checked
 * @return 0 if extension matched, 1 if not matched
***********************************************************************/
uint8_t file_extension_check(uint8_t* fname, uint8_t* extension)
{
	return strncmp(fname+strlen(fname)-strlen(extension),extension,strlen(extension));	
}

/***********************************************************************
 * @brief send_file()
 * This funtion is used to send a file to client via udp protocol
 * @param fname filename of the file to be sent
 * @return eof_check whether file exist or not
***********************************************************************/
int32_t send_file(uint8_t* fname,uint8_t* buffer,uint8_t* postdata)
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
		if(!file_extension_check(fname,txt_str0))
		{
			file_type=txt_str1;
		}
		else if(!file_extension_check(fname,png_str0))
		{
			file_type=png_str1;
		}
		else if(!file_extension_check(fname,jpg_str0))
		{
			file_type=jpg_str1;
		}
		else if(!file_extension_check(fname,gif_str0))
		{
			file_type=gif_str1;
		}
		else if(!file_extension_check(fname,css_str0))
		{
			file_type=css_str1;
		}
		else if(!file_extension_check(fname,js_str0))
		{
			file_type=js_str1;
		}
		else if(!file_extension_check(fname,html_str0))
		{
			file_type=html_str1;
		}
		else
		{
			eof_check=0;
			return eof_check;
		}
		if(method==GET)
		{
			sprintf(header,"HTTP/1.1 200 OK\r\n Content-Type:%s\r\nContent-Length:%d\r\n\r\n",file_type,file_size);
		}
		else if(method==POST)
		{
			sprintf(header,"HTTP/1.1 200 Ok\r\nContent-type:%s\r\nContent-size:%d\r\n\r\n<html><body><pre><h1>%s</h1></pre>\n\r",file_type,file_size,postdata);
		}		
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
		eof_check=1;
		fclose(fptr);
	}
	else
	{
		eof_check=0;
		//send_file("error500.txt",buffer);
		//syslog(SYSLOG_PRIORITY,"The file %s not found",fname);
	}
	return eof_check;
}

