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
    FILE *fp;
    int8_t *url_hash = MD5sum(url);
    int8_t filename[BUFFER_SIZE];
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
    if((fp = fopen(filename, "r")) != NULL)
    {
        getline(&line, &length, fp);
        sscanf(line, "%lu", &fileCreationTime); // Extracting the file creation time from the file
        exp_time = current_time - fileCreationTime; // Cache Expiration check
        if(exp_time < timeout)
        {
            fclose(fp);
            return 1;
        }
        else
        {
            fclose(fp);
            remove(filename);
            return 0;
        }
    }
    else
    {
        return 0;
    }
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
    sprintf(filename, "cache/hosts");
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


/***** Function to check for the requested URL in forbidden websites link *****/
int32_t checkForbiddenHost(int8_t  *hostname)
{
    FILE * fptr;
    int8_t * line=NULL;
    int8_t * temp=hostname;
    size_t length;
    int32_t found = 0;
    if((fptr = fopen("cache/Blacklist", "r")) != NULL)
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
    return found;
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

