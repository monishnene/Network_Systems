/*
    C socket server example, handles multiple clients using threads
*/
 
#include "proxy_support.h"
 
//the thread function
 
/*****Main Function *****/
int main(int argc, char* argv[]){
  int sockfd;
  unsigned long int timeout;
  struct sockaddr_in client_addr, proxy_addr;
  unsigned int length_client = sizeof(client_addr);
  int newsockfd;
  int pid;

  if (argc != 3)
  {
    printf ("\nUsage: <portNo> <timeout>\n");
    exit(1);
  }

/***** Creating the socket*****/
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("Error creating socket \n");
    exit(1);
  }


  bzero(&proxy_addr, sizeof(proxy_addr));
  proxy_addr.sin_family = AF_INET;
  proxy_addr.sin_port = htons(atoi(argv[1]));
  proxy_addr.sin_addr.s_addr = INADDR_ANY;

/*****binding the socket *****/
  if(bind(sockfd, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0){
	   printf("unable to bind socket\n");
     exit(1);
  }
  printf("**********Waiting for New Connection**********\n\n");

/*****Waiting for new connections *****/
  if(listen(sockfd, 1024) < 0){
    printf("*******Error in Listen*******\n");
    exit(1);
  }

  while(1){
    /***** Accepting new functions*****/
    if((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &length_client)) < 0){
      printf("Error in accept. \n");
      exit(1);
    }
    else{
      /**** FORK for handling multiple clients ****/
      pid = fork();

      if(pid<0){
        printf("Error in Fork\n");
      }

      //Fork for the recieved Request from Client
      if(pid==0){
        printf("**********New Connection at Port %d - Socket : %d**********\n\n", atoi(argv[1]), newsockfd);

        //Cache Expiration Timeout from Command Line
        timeout = atoi(argv[2]);

        //Function Calll to Respond to the Client Request
        response(newsockfd, timeout);

        close(newsockfd);
        exit(0);
      }
    }
  }
  return 0;
}
