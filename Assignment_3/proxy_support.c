/***********************************************************************
 * client_support.c
 * Network Systems CSCI 5273 Programming Assignment 1
 * Author: Monish Nene
 * Date: 09/09/2018
 * @brief This file has supporting functions for the server
 * Application file transfer using UDP protocol
***********************************************************************/
#include "proxy_support.h"

void md5_encode(int8_t *url,int8_t* url_encoded)
{
    uint8_t hash[MD5_DIGEST_LENGTH];
    int32_t i=0;
    MD5_CTX mdContext;
    MD5_Init (&mdContext);
    MD5_Update (&mdContext, url, strlen(url));
    MD5_Final (hash,&mdContext);
    for(i=0;i<MD5_DIGEST_LENGTH;i++)
    {
	snprintf(url_encoded+i,32,"%01x",hash[i]);
    }
}

int32_t checkCacheFile(int8_t *url)
{
    FILE *fptr;
    int8_t url_encoded[IP_ADDRESS_SIZE];	
    md5_encode(url,url_encoded);
    int8_t filename[DOMAIN_NAME_SIZE],status=0;
    int8_t buffer[BUFFER_SIZE];
    int8_t* line=NULL;
    size_t length;
    uint32_t fileCreationTime=0;
    uint32_t exp_time;
    time_t current_time;
    current_time = time(NULL); 
    bzero(filename, sizeof(filename));
    sprintf(filename, "cache/%s", url_encoded);
    printf("\nFilename in CacheFileCheck: %s", filename );
    if((fptr = fopen(filename, "r")) != NULL)
    {
        getline(&line, &length, fptr);
        sscanf(line, "%lu", &fileCreationTime);
        exp_time = current_time - fileCreationTime;
        if(exp_time < timeout)
        {
            status = 1;
	    printf("\nRecent Data found in Cache");
        }
        else
	{
	    printf("\nOld Data found in Cache");
            remove(filename);
        }
	fclose(fptr);
    }
    else
    {
	    printf("\nData not found in Cache");
    }
    return status;
}


int32_t checkCacheHost(int8_t *hostname, int8_t *ip)
{
    FILE* fp;
    int8_t * line=NULL;
    size_t length;
    int8_t  filename[BUFFER_SIZE];
    int32_t flag=0;
    bzero(filename, sizeof(filename));
    sprintf(filename, "Hostnames_IP");
    if((fp = fopen(filename, "r")) != NULL)
    {
        while((getline(&line, &length, fp)) != -1)
        {
            if(strstr(line, hostname))
            {
                sscanf(line, "%*[^ ]%*c%s", ip);
                flag = 1;
                break;
            }
        }
        fclose(fp);
    }
    return flag;
}

int32_t check_valid_ip(int8_t *hostname,void *socket_desc)
{
	int32_t newsockfd = *(int32_t*)socket_desc,nbytes;
	struct hostent *host_ptr;
	uint8_t i=0;
	host_ptr=gethostbyname(hostname);
	if(host_ptr!=NULL)
	{
		#ifdef DEBUG
		printf("\nReal Name->%s",host_ptr->h_name);
		while(*(host_ptr->h_aliases+i)!=NULL)
		{
			printf("\nAlias = %s",*(host_ptr->h_addr_list+i++));
		}		
		printf("\nIp found");
		return 1;
		#endif
	}
	else
	{
		nbytes = send(newsockfd, error404,strlen(error404), 0 );
    		shutdown(newsockfd,SHUT_RDWR);
    		close(newsockfd);
		printf("\nIP not found");
		return -404;	
	}
}

int32_t checkForbiddenHost(int8_t  *hostname,void *socket_desc)
{
    int32_t newsockfd = *(int32_t*)socket_desc,nbytes;
    FILE * fptr;
    int8_t * line=NULL;
    int8_t * temp=hostname;
    size_t length;
    int32_t found = 0;
    if((fptr = fopen("Blacklist", "r")) != NULL)
    {
        if(strchr(hostname, ':'))
        {
            sscanf(hostname, "%[^:]%*c", temp);
	}
        while((getline(&line, &length, fptr)) != -1)
        {
            if(strstr(line, temp))
            {
                found = 1;
                break;
            }
         }
         fclose(fptr);
    }
    if(found)
    {	
    	printf("Forbidden webpage->%s\n", hostname);
    	nbytes = send(newsockfd, error403,strlen(error403), 0 );
    	shutdown(newsockfd,SHUT_RDWR);
    	close(newsockfd);
    }
    return found;
}

int32_t check_valid_input(int8_t* buffer,int8_t* url)
{
	int8_t* method=(int8_t*)malloc(DATA_SIZE);
	int8_t* version=(int8_t*)malloc(DATA_SIZE);
	sscanf(buffer, "%s %s %s", method, url, version);
        if(strcmp(method, get_str) != 0)
        {
            printf("\nUnsupported Method %s",method);
	    return -400;
        }
        else if((strstr(url,https_str) != NULL) || (strstr(url,http_str) == NULL))
        {
            printf("\nUnsupported URL %s",url);
	    return -400;
        }
        else if((strcmp(version, v0) != 0) && (strcmp(version, v1) != 0))
        {
            printf("\nUnsupported Version %s",version);
	    return -400;
        }
	return 0;
}

