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
int32_t checkCacheFile(int8_t *url, uint32_t timeout)
{
    FILE *fp;
    int8_t *url_hash = MD5sum(url);
    int8_t filename[MAXBUFSIZE];
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
    sprintf(filename, "./cache/%s", url_hash);
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
    int8_t  filename[MAXBUFSIZE];
    int32_t flag=0;
    bzero(filename, sizeof(filename));
    sprintf(filename, "./cache/hosts");
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
        return 0;`
    }
}


/***** Function to check for the requested URL in forbidden websites link *****/
int32_t checkForbiddenHost(int8_t  *hostname, int8_t  *forbid_ip)
{
    FILE * fp;
    int8_t * line=NULL;
    size_t length;
    int32_t flag = 0;
    if((fp = fopen("forbidden", "r")) != NULL)
    {
        if(strchr(hostname, ':'))
        {  //Check for hostname Formatting
            sscanf(hostname, "%[^:]%*c", forbid_ip);
            while((getline(&line, &length, fp)) != -1)
            {
                if(strstr(line, forbid_ip))
                {
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
        else
        {
            while((getline(&line, &length, fp)) != -1)
            {
                if(strstr(line, hostname))
                {
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
    int8_t  url[MAXBUFSIZE];
    int8_t url_inc[MAXBUFSIZE]; //URL Incomplete
    int8_t server_req[MAXBUFSIZE];
    int8_t buffer[MAXBUFSIZE];
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

/***** Response Function for Client Requests *****/
void response(int32_t newsockfd, uint32_t timeout)
{
    //int32_t newsockfd = a;
    int8_t filename[MAXBUFSIZE];
    int32_t sockfd1;
    int8_t method[MAXBUFSIZE];
    int8_t url[MAXBUFSIZE];
    int8_t version[MAXBUFSIZE];
    int8_t ip[128] = "";
    int8_t forbid_ip[128] = "";
    int8_t port[32] = "80";
    int8_t hostname[MAXBUFSIZE];
    struct hostent *server_hp;							// to represent entry in host database
    FILE *fp;
    struct sockaddr_in server;
    int32_t nbytes;
    int8_t buffer[MAXBUFSIZE];
    int8_t req_buffer[MAXBUFSIZE];
    int8_t *url_hash;
    int8_t* line=NULL;
    size_t length;
    int32_t flag = 0;
    int32_t pid2;
    bzero(buffer, sizeof(buffer));
    bzero(req_buffer, sizeof(req_buffer));
    while((nbytes = recv(newsockfd, buffer, sizeof(buffer), 0)))
    {
        //printf("Request Buffer:\n%s\n", buffer);
        strncpy(req_buffer, buffer, nbytes);
        bzero(method, sizeof(method));
        bzero(url, sizeof(method));
        bzero(version, sizeof(version));
        sscanf(buffer, "%s %s %s", method, url, version);
        printf("Method: %s \tURL: %s \tVersion:%s\n", method, url, version );
        url_hash = MD5sum(url); //Calling MD5sum function to get hash value to create filename
        //Check for GET request
        if(strcmp(method, "GET") != 0)
        {
            bzero(buffer, sizeof(buffer));
            sprintf(buffer, ERR_METHOD, (int)strlen("<html><body><H1>Error 400 Bad Request: Method Not Supported </H1></body></html>"));
            printf("Error Buffer\n%s\n", buffer);
            nbytes = send(newsockfd, buffer, strlen(buffer), 0 );
            continue;
        }
        //Check for HTTP request
        else if((strstr(url, "https") != NULL) || (strstr(url, "http") == NULL))
        {
            bzero(buffer, sizeof(buffer));
            sprintf(buffer, ERR_URL, (int)strlen("<html><body><H1>Error 400 Bad Request: Invalid URL </H1></body></html>"));
            printf("Error Buffer\n%s\n", buffer);
            nbytes = send(newsockfd, buffer, strlen(buffer), 0 );
            continue;
        }
        //Check for Valid HTTP Version
        else if((strcmp(version, "HTTP/1.0") != 0) && (strcmp(version, "HTTP/1.1") != 0))
        {
            bzero(buffer, sizeof(buffer));
            sprintf(buffer, ERR_VERSION, (int)strlen("HTTP/1.1 400 Bad Request\n\rContent-Type: text/html\nContent-Length: %d\n\r\n<html><body><H1>Error 400 Bad Request: Invalid HTTP Version </H1></body></html>"));
            printf("Error Buffer\n%s\n", buffer);
            nbytes = send(newsockfd, buffer, strlen(buffer), 0 );
            continue;
        }
        else
        {
            sscanf(url, "%*[^/]%*c%*c%[^/]", hostname);
            printf("Hostname: %s\n", hostname );
            //Function call to check for forbiddedn(blocked) website
            int32_t checkForbidden = checkForbiddenHost(hostname, forbid_ip);
            if(checkForbidden == 1)
            {
                bzero(buffer, sizeof(buffer));
                sprintf(buffer, ERR_BLOCKED, (int)strlen("<html><body>ERROR 403 Forbidden</body></html>"));
                printf("Error Buffer\n%s\n", buffer);
                nbytes = send(newsockfd, buffer, strlen(buffer), 0 );
                continue;
            }
            //Function call to check whether file is present in the cache
            int32_t cacheFilePresent = checkCacheFile(url, timeout);
            if(cacheFilePresent == 1)
            {
                printf("\n*****Page found in Cache Socket:%d*****\n", newsockfd );
                FILE *fp;
                bzero(filename, sizeof(filename));
                sprintf(filename, "./cache/%s", url_hash);
                fp = fopen(filename, "r");
                getline(&line, &length, fp);
                //sending file to client if found in cache
                bzero(buffer, sizeof(buffer));
                while((nbytes = fread(buffer, 1, sizeof(buffer), fp)))
                {
                    send(newsockfd, buffer, nbytes, 0);
                    bzero(buffer, sizeof(buffer));
                }
                fclose(fp);
                continue;
            }
            else
            {
                printf("\n*****Page Not found in Cache:%d*****\n", newsockfd);
                //Check for a formatting possibility for hostname
                if(strchr(hostname, ':'))
                {
                    bzero(port, sizeof(port));
                    sscanf(hostname, "%[^:]%*c%[^/]", ip, port);
                    bzero(&server,sizeof(server));               //zero the struct
                    server.sin_family = AF_INET;                 //address family
                    server.sin_port = htons(atoi(port));      //sets port to network byte order
                    server.sin_addr.s_addr = inet_addr(ip); //sets remote IP address
                }
                else
                {
                    //Function call to check for Hostname in cache to save the DNS query
                    int32_t checkHostPresent = checkCacheHost(hostname, ip);
                    if(checkHostPresent==1)
                    {
                        printf("\n*******Host Present in Cache*******\n");
                        bzero(filename, sizeof(filename));
                        sprintf(filename, "./cache/hosts");
                        //fp = fopen(filename, "ab");
                        bzero(&server,sizeof(server));               //zero the struct
                        server.sin_family = AF_INET;                 //address family
                        server.sin_port = htons(atoi(port));      //sets port to network byte order
                        server.sin_addr.s_addr = inet_addr(ip); //sets remote IP address
                    }
                    else
                    {
                        printf("\n*******Host Not Present in Cache*******\n");
                        bzero(&server,sizeof(server));               //zero the struct
                        server.sin_family = AF_INET;                 //address family
                        server.sin_port = htons(atoi(port));      //sets port to network byte order
                        //server.sin_addr.s_addr = inet_addr(hostname); //sets remote IP address
                        server_hp = gethostbyname(hostname);					 // Return information about host in argv[1]
                        bcopy((int8_t*)server_hp->h_addr, (int8_t*)&server.sin_addr, server_hp->h_length);
                        //Check for Valid Server
                        if(server_hp < 0)
                        {
                            bzero(buffer, sizeof(buffer));
                            sprintf(buffer, ERR_SERVERNOTFOUND, (int)strlen("<html><body><H1>Error 400 Bad Request: Server Not Found </H1></body></html>"));
                            printf("Error Buffer\n%s\n", buffer);
                            nbytes = send(newsockfd, buffer, strlen(buffer), 0 );
                            continue;
                        }
                        else
                        {
                            bzero(filename, sizeof(filename));
                            sprintf(filename, "./cache/hosts");
                            fp = fopen(filename, "ab");
                            bzero(buffer, sizeof(buffer));
                            sprintf(buffer, "%s %s\n", hostname, inet_ntoa(server.sin_addr));
                            fwrite(buffer, 1, strlen(buffer), fp);
                            fclose(fp);
                        }
                    }
                }
                /***** Creating the socket to fetch data from the remote server*****/
                if ((sockfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                    printf("Error creating socket at proxy \n");
                    continue;
                }
                //Connecting to remote server
                if((connect(sockfd1, (struct sockaddr *)&server, sizeof(server))) < 0)
                {
                    printf("Error in Connect to the server. \n");
                    continue;
                }
                //Sending Request to Remote Server
                send(sockfd1, req_buffer, strlen(req_buffer), 0);
                bzero(buffer, sizeof(buffer));
                bzero(filename, sizeof(filename));
                sprintf(filename, "./cache/%s", url_hash);
                fp = fopen(filename, "ab");
                if(fp < 0)
                {
                    printf("Error Creating Cache file\n");
                    exit(1);
                }
                time_t current_time1;
                current_time1 = time(NULL); //Get Current time
                fprintf(fp, "%lu\n", current_time1); //Appending Current Time to the Cached File
                bzero(buffer, sizeof(buffer));
                while((nbytes = recv(sockfd1, buffer, sizeof(buffer), 0)))
                {
                    printf("\nbuffer_filled=%d, buffer_len=%ld ",nbytes,strlen(buffer));
                    //Check for Links to be Prefetched
                    if(strstr(buffer, "<html"))
                    {
                        flag = 1; //Flag-Set for a Prefetching Link Found
                    }
                    //Sending the data recieved for the request to the client
                    send(newsockfd, buffer, nbytes, 0);
                    fwrite(buffer, 1, nbytes, fp);
                    bzero(buffer, sizeof(buffer));
                }
                fclose(fp);
                //Fork if Prefetch Link Found
                if(flag==1)
                {
                    pid2 = fork();
                    if(pid2 == 0)
                    {
                        int32_t ret = linkPrefetch(inet_ntoa(server.sin_addr), filename, hostname, port );
                        exit(0);
                    }
                }
            }
        }
    }
}


