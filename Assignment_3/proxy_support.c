/***********************************************************************
 * client_support.c
 * Network Systems CSCI 5273 Programming Assignment 1
 * Author: Monish Nene
 * Date: 09/09/2018
 * @brief This file has supporting functions for the server
 * Application file transfer using UDP protocol
***********************************************************************/
#include "proxy_support.h"

/***** Function to Calculate MD5 Hash for cached filename *****/
int8_t* MD5sum(int8_t *url)
{
    uint8_t hash_hex[MD5_DIGEST_LENGTH];
    int32_t i;
    int8_t *md5string =(int8_t*) malloc(sizeof(int8_t)*256);
    MD5_CTX mdContext;
    MD5_Init (&mdContext);
    MD5_Update (&mdContext, url, strlen(url));
    MD5_Final (hash_hex,&mdContext);
    for(i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        snprintf(&md5string[i*2], 32, "%02x", hash_hex[i]);
    }
    return (int8_t *)md5string;
}

/***** Function to check for the presence of cached file *****/
int32_t checkCacheFile(int8_t *url)
{
    //int32_t newsockfd = *(int32_t*)socket_desc,nbytes;
    FILE *fptr;
    int8_t *url_hash = MD5sum(url);
    int8_t filename[DOMAIN_NAME_SIZE],status=0;
    int8_t buffer[BUFFER_SIZE];
    int8_t* line=NULL;
    size_t length;
    uint32_t fileCreationTime=0;
    uint32_t exp_time;
    time_t current_time;
    current_time = time(NULL);  // fetching the current time for cache-expiration check

    if(url_hash == 0)
    {
        printf("Error Calculating Hash value\n");
    }
    bzero(filename, sizeof(filename));
    sprintf(filename, "cache/%s", url_hash);
    printf("Filename in CacheFileCheck: %s\n", filename );
    if((fptr = fopen(filename, "r")) != NULL)
    {
        getline(&line, &length, fptr);
        sscanf(line, "%lu", &fileCreationTime); // Extracting the file creation time from the file
        exp_time = current_time - fileCreationTime; // Cache Expiration check
        if(exp_time < timeout)
        {
            status = 1;
        }
        else
        {
            remove(filename);
	    fclose(fptr);
        }
    }
    /*if(status)
    {
    	printf("\n*****Page found in Cache Socket:%d*****\n", newsockfd );
                getline(&line, &length, fptr);
                //sending file to client if found in cache
                bzero(buffer, sizeof(buffer));
                while((nbytes = fread(buffer, 1, sizeof(buffer), fptr)))
                {
                    send(newsockfd, buffer, nbytes, 0);
                    bzero(buffer, sizeof(buffer));
                }
    		shutdown(newsockfd,SHUT_RDWR);
    		close(newsockfd);
                fclose(fptr);
    }*/
    return status;
}


/***** Check for the presence of Hostname in cached file for hosts *****/
int32_t checkCacheHost(int8_t *hostname, int8_t *ip)
{
    FILE* fp;
    int8_t * line=NULL;
    size_t length;
    int8_t  filename[BUFFER_SIZE];
    int32_t flag=0;
    bzero(filename, sizeof(filename));
    sprintf(filename, "Hostnames_IP");
    if((fp = fopen(filename, "r")) == NULL)
    {
        return 0;
    }
    else
    { //Check for the presence of host in cache
        while((getline(&line, &length, fp)) != -1)
        {
            if(strstr(line, hostname))
            {
                sscanf(line, "%*[^ ]%*c%s", ip);
                flag =1;
                break;
            }
        }
        if(flag==1)
        {
            fclose(fp);
            return 1;
        }
        fclose(fp);
        return 0;
    }
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

/***** Function to check for the requested URL in forbidden websites link *****/
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


/***** Link Prefetching: Extra-Credits Part *****/
int32_t linkPrefetch(int8_t * prefetch_ip, int8_t * filename, int8_t * hostname, int8_t * port )
{
    FILE * fp1;
    FILE * fp2;
    int8_t * line = NULL;
    size_t length;
    int8_t * url_full;
    int8_t  url[BUFFER_SIZE];
    int8_t url_inc[BUFFER_SIZE]; //URL Incomplete
    int8_t server_req[BUFFER_SIZE];
    int8_t buffer[BUFFER_SIZE];
    int32_t nbytes = 0;
    int8_t* url_hash = NULL;
    int8_t cacheFilename[64];
    struct sockaddr_in server_add_prefetch;
    int32_t sockfd2;
    int8_t * ret_strstr = NULL;
    // Initializing new struct for Prefetching links
    bzero(&server_add_prefetch,sizeof(server_add_prefetch));               //zero the struct
    server_add_prefetch.sin_family = AF_INET;                 //address family
    server_add_prefetch.sin_port = htons(atoi(port));      //sets port to network byte order
    server_add_prefetch.sin_addr.s_addr = inet_addr(prefetch_ip); //sets remote IP address
    if((fp1 = fopen(filename, "r")) != NULL)
    {
        while((getline(&line, &length, fp1) != -1))
        {
            bzero(cacheFilename, sizeof(cacheFilename));
            bzero(url, sizeof(url));
            bzero(url_inc, sizeof(url_inc));
            bzero(server_req, sizeof(server_req));
            bzero(buffer, sizeof(buffer));
            if((ret_strstr = strstr(line, "href")))
            {  //Check for links to be prefetched
                if ((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                    printf("Error creating socket at proxy \n");
                    exit(1);
                }
                if((connect(sockfd2, (struct sockaddr *)&server_add_prefetch, sizeof(server_add_prefetch))) < 0)
                {
                    printf("Error in Connect to the server. \n");
                    exit(1);
                }
                if((url_full = strstr(line, "http://")))
                {   //Check for link formatting
                    sscanf(url_full, "%[^\"]", url);
                }
                else
                {
                    sscanf(ret_strstr, "%*[^=]%*c%*c%[^\"]", url_inc );
                    if(url_inc[0] == '/')
                    {
                        sprintf(url, "http://%s%s", hostname, url_inc);
                    }
                    else
                    {
                        sprintf(url, "http://%s/%s", hostname, url_inc);
                    }
                }
                sprintf(server_req, "GET %s HTTP/1.0\r\n\r\n", url);  //Creating Request for Links to be prefetched
                url_hash = MD5sum(url); //Calling MD5 function to create the filename using hash value
                sprintf(cacheFilename, "./cache/%s", url_hash );
                send(sockfd2, server_req, strlen(server_req), 0);
                fp2 = fopen(cacheFilename, "ab");
                if(fp2 < 0)
                {
                    printf("Error Creating Cache file in Prefetch\n");
                    exit(1);
                }
                time_t current_time2;
                current_time2 = time(NULL);
                fprintf(fp2, "%lu\n", current_time2);   //Appending the File Creation Tiem for the cached-file
                while((nbytes = recv(sockfd2, buffer, sizeof(buffer), 0)))
                { //Recieving File to be cached
                    printf("Prefetch Link Received Bytes: %d\n", nbytes);
                    fwrite(buffer, 1, nbytes, fp2);
                    bzero(buffer, sizeof(buffer));
                }
                fclose(fp2);
            }
        }
        fclose(fp1);
        return 0;
    }
    else
    {
        return 1;
    }
}

