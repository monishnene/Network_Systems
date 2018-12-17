/***********************************************************************
 * server_support.c
 * Network Systems CSCI 5273 Programming Assignment 2
 * Author: Monish Nene
 * Date: 10/09/2018
 * @brief This file has supporting functions for the server
 * Application file transfer using TCP protocol
***********************************************************************/
#include "dfs_support.h"
/**
 * error - wrapper for perror
 */
void error(uint8_t *msg)
{
	perror(msg);
    	exit(0);
}

short_signals authorization_check(int8_t *username, int8_t *password)
{
    FILE* fptr;	
    int8_t temp_password[20]; 
    int8_t* line=NULL;
    size_t length;
    if((fptr = fopen(conf_filename, "r")) != NULL)
    {
        while((getline(&line, &length, fptr)) != -1)
        {
	    //printf("\n line = %s",line);
            if(strstr(line, username))
            {
                sscanf(line, "%s %s",username,temp_password);
		//printf("\n username = %s,temp_password = %s,password = %s",username,temp_password,password);
                if(strcmp(temp_password,password))
		{
        		fclose(fptr);
			return incorrect_password;
		}
		else
		{
			fclose(fptr);
			return approved;
		}
            }
	    fclose(fptr);
	    return user_not_found;
        }
    }
    else
    {
	printf("\n%s conf file not found",conf_filename);
	fclose(fptr);
	return file_not_found;
    }
}


/***********************************************************************
 * @brief search_str()
 * Search a needle string in another haystack string.
 * @param haystack string to be searched in
 * @param needle small string to be searched
 * @return 1 if found 0 if not found
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


uint8_t act_server(commands command)
{
	int32_t error_check=0;
	uint8_t i=0,receiver_ready=1;
	write(sock,&receiver_ready,sizeof(receiver_ready));
	switch(command)
	{
		case put:
		{
			error_check=receive_file();
			error_check=receive_file();			
			break;
		}
		case get:
		{
			while(1)
			{			
				read(sock,&receiver_ready,sizeof(receiver_ready));
				if(receiver_ready == 0)
				{
					break;
				}
				else
				{	
					bzero(filename,20);
					write(sock,&receiver_ready,sizeof(receiver_ready));
					read(sock,filename,20);
					send_file();
				}
			}
			break;
		}
		case mkdir:
		{
			error_check=folder_creation();
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

uint8_t folder_creation()
{
	int8_t mkdir_str[20];
	sprintf(mkdir_str,"mkdir DFS%d/%s/%s/",server_id,username,filename);	
	printf("\nFolder created by command %s",mkdir_str);
	system(mkdir_str);
}

uint8_t receive_file()
{
	FILE* fptr;
	int32_t data_bytes=1;
	uint8_t receiver_ready=1;
	uint8_t temp_filename[PATH_SIZE],temp[20];
	//printf("\npath=%s",temp_filename);
        bzero(buffer, sizeof(buffer));
	bzero(temp,20);
	data_bytes = write(sock,&receiver_ready,sizeof(receiver_ready));
        data_bytes = read(sock,&file_size,sizeof(file_size));
	data_bytes = write(sock,&receiver_ready,sizeof(receiver_ready));
	data_bytes = read(sock,temp,20);
	data_bytes = write(sock,&receiver_ready,sizeof(receiver_ready));
	sprintf(temp_filename,"%s%s",path,temp);
	fptr = fopen(temp_filename, "w");
	printf("\nFile size to be received = %d",file_size);
	data_bytes = read(sock,buffer,file_size);
	if(data_bytes==file_size)
	{
		printf("\nFile size matched filename=%s",temp_filename);
		fwrite(buffer, 1, data_bytes, fptr);
	}
	fclose(fptr);
	return data_bytes;
}

uint8_t send_file()
{	
	FILE* fptr;
	uint8_t temp_filename[50],temp[20];
	int32_t n=0,eof_check=0,file_size=0;
	uint8_t data[PACKET_SIZE],i=0,file_present=0,receiver_ready=0;
	bzero(temp_filename,50);
	sprintf(temp_filename,"%s%s",path,filename);
	printf("\nServere %d Trying to open file %s",server_id+1,temp_filename);
	fptr = fopen(temp_filename, "r");
	if(fptr != NULL)
	{
		file_present=1;
		write(sock,&file_present,sizeof(file_present));
	}
	else
	{
		write(sock,&file_present,sizeof(file_present));
		return file_size;
	}
	fseek(fptr,0,SEEK_END);
	file_size=ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	n = read(sock,&receiver_ready,sizeof(receiver_ready));
	write(sock,&file_size,sizeof(file_size));
	n = read(sock,&receiver_ready,sizeof(receiver_ready));
	bzero(buffer,BUFFER_SIZE);
	printf("\nReading file %s of size %d",temp,file_size);
	n=fread(buffer,1,file_size,fptr);
	n=write(sock,buffer,file_size);			
	printf("\nFile %s with %d bytes sent by server %d",temp,file_size,server_id+1);	
	fclose(fptr);
	return file_size;
}