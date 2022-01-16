#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

int DoClientProcessing(int sockfd);
char buffer[256];
int main(int argc, char *argv[]) {
   int sockfd, portno;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
	
	server = gethostbyname(argv[1]);
    
	if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }	
	
   	portno = atoi(argv[2]);
   
   int firsttime =0;

   int t = 0;
	while(t >= 0){
	   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
	   if (sockfd < 0) {
	      perror("ERROR opening socket");
	      exit(1);
	   }	
		
	   bzero((char *) &serv_addr, sizeof(serv_addr));
	   serv_addr.sin_family = AF_INET;
	   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	   serv_addr.sin_port = htons(portno);
	
	
	  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
	      perror("ERROR connecting");
	      return -1;
	  	 }

          if (firsttime ==0){
		int ss = 0;
		int converted_number = htonl(ss);
		int f = write(sockfd, &converted_number, sizeof(converted_number));
		bzero(buffer,256);
		if (read(sockfd, buffer, 255)>0)
		{
			char rbuf[5];
                        strncpy(rbuf,buffer,5);
		   	printf("%s\n ",rbuf);
		}
		firsttime =1;
	}
	else{
		t = DoClientProcessing(sockfd);
	}
			
		close(sockfd);
	
	}
   	
   return 0;
}

int DoClientProcessing(int sockfd){
	int n, player;
		printf("Type in your choice of 'rock'=1 'paper'=2 or 'scissor'=3'\n");
	   do{
	   		printf("Please enter value from 1 to 3\n");
	   		scanf ( "%i", &player );	
	   }while( player < 1 || player > 3);
	   
	   int converted_number = htonl(player);

		n = write(sockfd, &converted_number, sizeof(converted_number));
	   
	   if (n < 0) {
	      perror("ERROR writing to socket");
	      return -1;
	   }
	   
	   bzero(buffer,256);
	   n = read(sockfd, buffer, 255);
	   
	   if (n < 0) {
	      perror("ERROR reading from socket");
	      return -1;
	   }
	   printf("%s\n",buffer);
	   	   
	   return 0;
}

