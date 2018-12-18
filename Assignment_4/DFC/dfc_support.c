/***********************************************************************
 * dfc_support.c
 * Network Systems CSCI 5273 Programming Assignment 4
 * Author: Monish Nene
 * Date: 12/09/2018
 * @brief This file has the functions for the DFS Client
 * Application Distributed file system
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

/***********************************************************************
 * @brief remove_newline_char()
 * This funtion is used to remove newline character from a string
 * @param pointer to the string to be processed
***********************************************************************/
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
        bzero(foldername,20);
	if(!strncmp(input,get_str,strlen(get_str)))
	{
		command_caught=get;
		sscanf(input,"%s %s %s",junk,filename,foldername);
	}
	else if(!strncmp(input,put_str,strlen(put_str)))
	{
		command_caught=put;
		sscanf(input,"%s %s %s",junk,filename,foldername);
	}
	else if(!strncmp(input,ls_str,strlen(ls_str)))
	{
		command_caught=list;
		sscanf(input,"%s %s",junk,foldername);
	}
	else if(!strncmp(input,mkdir_str,strlen(mkdir_str)))
	{
		command_caught=mkdir;
		sscanf(input,"%s %s",junk,foldername);
	}
	return command_caught;
}

/***********************************************************************
 * @brief md5sum_hash()
 * This funtion is used to implement md5sum on a file and get a hash value
 * @return hash value
***********************************************************************/
uint8_t md5sum_hash()
{
	uint8_t temp_command[40],hash_sum[PACKET_SIZE],i=0;
	int32_t n=0,sum=0;
	sprintf(temp_command,"md5sum %s>>md5sum.txt",filename);
	system(temp_command);
	FILE* fptr=fopen(filename, "r");
	n=fread(hash_sum,1,PACKET_SIZE,fptr);
	for(i=0;i<PACKET_SIZE;i++)
	{
		sum+=hash_sum[i];
	}
	i=sum%4;
	return i;
}

/***********************************************************************
 * @brief split_file()
 * This funtion is used to split a file into 4 parts to send toservers.
 * @return error check
***********************************************************************/
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

/***********************************************************************
 * @brief merge_file()
 * This funtion is used to merge 4 parts of a file received from servers.
 * @return error check
***********************************************************************/
int32_t merge_file()
{
	int32_t data_bytes=0,n=0,eof_check=0,file_size=0,split[5];
	uint8_t temp_filename[20],i=0;
	sprintf(temp_filename,"Get_%s",filename);
	printf("\nMerging received files into %s",temp_filename);
	FILE* fptr=fopen(temp_filename, "w");
	FILE* split_fptr;
	for(i=0;i<TOTAL_SERVERS;i++)
	{
		bzero(buffer,BUFFER_SIZE);
		bzero(temp_filename,20);
		sprintf(temp_filename,"%s.%d",filename,i+1);
		split_fptr = fopen(temp_filename, "r");
		if(split_fptr == NULL)
		{
			printf("\nIncompleted files for merging into %s",filename);
			printf("\nFile %s is missing",temp_filename);
			fclose(fptr);
			return 0;
		}
		fseek(split_fptr,0,SEEK_END);
		file_size=ftell(split_fptr);
		fseek(split_fptr,0,SEEK_SET);
		n=fread(buffer,1,file_size,split_fptr);
		//printf("\nmerge filename = %s",temp_filename);
		fwrite(buffer, 1, file_size,fptr);
		fclose(split_fptr);
	}

	for(i=0;i<TOTAL_SERVERS;i++)
	{
		sprintf(temp_filename,"%s.%d",filename,i+1);
		remove(temp_filename);
	}
	fseek(fptr,0,SEEK_END);
	file_size=ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	fclose(fptr);
	return file_size;
}

/***********************************************************************
 * @brief act_client()
 * This funtion is used to take action according to command.
 * @param command to be executed
 * @return error check
***********************************************************************/
uint8_t act_client(commands command)
{
	int32_t error_check=0,n=0;
	uint8_t i=0,hash_value=0,file_counter=0,receiver_ready=1,file_present=0,j=0;
	uint8_t temp_filename[20];
	for(i=0;i<TOTAL_SERVERS;i++)
	{
		read(web_socket[i],&receiver_ready,sizeof(receiver_ready));
	}
	switch(command)
	{
		case get:
		{
			for(i=0;i<TOTAL_SERVERS;i++)
			{
				sprintf(temp_filename,"%s.%d",filename,i+1);
				for(j=0;j<TOTAL_SERVERS;j++)
				{
					printf("\nTrying to get file %s from server %d",temp_filename,j+1);
					write(web_socket[j],&receiver_ready,sizeof(receiver_ready));
					read(web_socket[j],&receiver_ready,sizeof(receiver_ready));
					write(web_socket[j],temp_filename,20);
					read(web_socket[j],&file_present,sizeof(file_present));
					if(file_present)
					{
						printf("\nReceving file %s from server %d",temp_filename,j+1);
						file_present=0;
						error_check=receive_file(temp_filename,j);
						break;
					}
					else
					{
						printf("\nFile %s not present with server %d",temp_filename,j+1);
					}
				}
			}
			receiver_ready=0;
			for(i=0;i<TOTAL_SERVERS;i++)
			{
				write(web_socket[i],&receiver_ready,sizeof(receiver_ready));
			}
			merge_file();
			for(i=0;i<TOTAL_SERVERS;i++)
			{
				sprintf(temp_filename,"%s.%d",filename,i+1);
				remove(temp_filename);
			}
			break;
		}
		case put:
		{
			error_check=split_file();
			hash_value=md5sum_hash();
			printf("\nHash value = %d",hash_value);
			for(i=0;i<TOTAL_SERVERS;i++)
			{
				sprintf(temp_filename,"%s.%d",filename,pair_table[hash_value][i][0]);
				error_check=send_file(temp_filename,i);
				sprintf(temp_filename,"%s.%d",filename,pair_table[hash_value][i][1]);
				error_check=send_file(temp_filename,i);
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
			for(i=0;i<TOTAL_SERVERS;i++)
			{
				error_check=simple_receive_file(i);
			}
			bzero(filename,20);
			sprintf(filename,"ls.txt");
			merge_file();
			list_analysis();
			break;
		}
		default:
		{
			printf("\nInvalid Command");
		}
	}
	return error_check;
}

/***********************************************************************
 * @brief list_analysis()
 * This funtion is used for analysis of list file
***********************************************************************/
void list_analysis()
{
	FILE* fptr;
	uint32_t i=0,sum=0,final=0,hash=0,j=0;
	uint8_t hash_table[HASH_SIZE][4];
	uint8_t shown[HASH_SIZE];
    	int8_t name[20],number=0,temp[20];
   	int8_t* line=NULL;
    	size_t length;
	for(i=0;i<HASH_SIZE;i++)
	{
		for(j=0;j<4;j++)
		{
			hash_table[i][j]=0;
		}
	}
	bzero(shown,HASH_SIZE);
    	if((fptr = fopen("Get_ls.txt", "r")) != NULL)
    	{
        	while((getline(&line, &length, fptr)) != -1)
        	{
			bzero(name,20);
			sscanf(line, "%s",name);
			length=strlen(name);
			number=name[length-1]-'1';
			name[length-1]=0;
			name[length-2]=0;
			sum=0;
			i=0;
			while(name[i]!=0)
			{
				sum+=name[i++];
			}
			hash=sum%HASH_SIZE;
			hash_table[hash][number]=1;
		}
		fseek(fptr,0,SEEK_SET);
		printf("\n\nList Results");
		while((getline(&line, &length, fptr)) != -1)
		{
			bzero(name,20);
			sscanf(line, "%s",name);
			length=strlen(name);
			number=name[length-1]-'1';
			name[length-1]=0;
			name[length-2]=0;
			sum=0;
			i=0;
			while(name[i]!=0)
			{
				sum+=name[i++];
			}
			final=0;
			hash=sum%HASH_SIZE;
			for(i=0;i<4;i++)
			{
				final+=hash_table[hash][i];
			}
			if((final==4)&&(shown[hash]==0))
			{
				printf("\n%s",name);
				shown[hash]=1;
			}
			else if(shown[hash]==0)
			{
				printf("\n%s (incomplete)",name);
				shown[hash]=1;
			}
		}
	}
	fclose(fptr);
}

/***********************************************************************
 * @brief simple_receive_file()
 * This funtion is used to receive file from server
 * @param server ID from whom the file is to be received
 * @return error check
***********************************************************************/
uint8_t simple_receive_file(uint8_t server_ID)
{
	FILE* fptr;
	int32_t data_bytes=1,file_size=0;
	uint8_t receiver_ready=1;
	uint8_t temp[20];
	//printf("\npath=%s",temp_filename);
        bzero(buffer, sizeof(buffer));
	bzero(temp,20);
	data_bytes = write(web_socket[server_ID],&receiver_ready,sizeof(receiver_ready));
        data_bytes = read(web_socket[server_ID],&file_size,sizeof(file_size));
	data_bytes = write(web_socket[server_ID],&receiver_ready,sizeof(receiver_ready));
	data_bytes = read(web_socket[server_ID],temp,20);
	data_bytes = write(web_socket[server_ID],&receiver_ready,sizeof(receiver_ready));
	fptr = fopen(temp, "w");
	printf("\nFile size to be received = %d",file_size);
	data_bytes = read(web_socket[server_ID],buffer,file_size);
	if(data_bytes==file_size)
	{
		printf("\nFile size matched filename=%s",temp);
		fwrite(buffer, 1, data_bytes, fptr);
	}
	fclose(fptr);
	return data_bytes;
}

/***********************************************************************
 * @brief  send_file()
 * This funtion is used to send file to server
 * @param file to be sent
 * @param server ID to whom the file is to be sent
 * @return error check
***********************************************************************/
uint8_t send_file(uint8_t* split_filename,uint8_t server_ID)
{
	int32_t data_bytes=0,n=0,eof_check=0,file_size=0;
	uint8_t data[PACKET_SIZE],i=0,receiver_ready=0;
	FILE* fptr=fopen(split_filename, "r");
	fseek(fptr,0,SEEK_END);
	file_size=ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	n = read(web_socket[server_ID],&receiver_ready,sizeof(receiver_ready));
	write(web_socket[server_ID],&file_size,sizeof(file_size));
	n = read(web_socket[server_ID],&receiver_ready,sizeof(receiver_ready));
	write(web_socket[server_ID],split_filename,20);
	n = read(web_socket[server_ID],&receiver_ready,sizeof(receiver_ready));
	bzero(buffer,BUFFER_SIZE);
	n=fread(buffer,1,file_size,fptr);
	if(n==file_size)
	{
		printf("\nFile %s with %d bytes sent to server %d",split_filename,file_size,server_ID+1);
		write(web_socket[server_ID],buffer,file_size);
	}
	else
	{
		printf("\nFile size error file_size = %d, n = %d",file_size,n);
	}
	fclose(fptr);
	return file_size;
}

/***********************************************************************
 * @brief receive_file()
 * This funtion is used to receive file from server
 * @param filename to be saved
 * @param server ID from whom the file is to be received
 * @return error check
***********************************************************************/
uint8_t receive_file(uint8_t* temp_filename,uint8_t server_ID)
{
	FILE* fptr;
	int32_t data_bytes=1,file_size=0;
	uint8_t receiver_ready=1;
        bzero(buffer, sizeof(buffer));
	data_bytes = write(web_socket[server_ID],&receiver_ready,sizeof(receiver_ready));
        data_bytes = read(web_socket[server_ID],&file_size,sizeof(file_size));
	printf("\nReceiving File = %s from server %d",temp_filename,server_ID+1);
	data_bytes = write(web_socket[server_ID],&receiver_ready,sizeof(receiver_ready));
	fptr = fopen(temp_filename, "w");
	printf("\nFile size to be received = %d",file_size);
	data_bytes = read(web_socket[server_ID],buffer,file_size);
	if(data_bytes==file_size)
	{
		printf("\nFile %s received from server %d",temp_filename,server_ID+1);
		fwrite(buffer, 1, data_bytes, fptr);
	}
	fclose(fptr);
	return data_bytes;
}

