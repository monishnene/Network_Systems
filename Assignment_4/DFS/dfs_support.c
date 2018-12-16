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
	    printf("\n line = %s",line);
            if(strstr(line, username))
            {
                sscanf(line, "%s %s",username,temp_password);
		printf("\n username = %s,temp_password = %s,password = %s",username,temp_password,password);
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

