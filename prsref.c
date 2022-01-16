#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZ 100

int DoServerProcessing(int newsockfd);
void child();
int win ,N ,winComp;
int pd[2];
char *SendMessage; 
 
int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   char m;
   
   if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
   
   N = atoi(argv[1]);
   portno = atoi(argv[2]);
   
   printf("I am the referee with PID %5d waiting for game request at port %d\n", getpid(), portno);
   printf("I am player 1 with PID %d\n", getpid());
   
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   int i = 1;
   win = 0;
   winComp =0;
   char str[512];
   while (i <= N) {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
      if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
      }
     
      int rw =  DoServerProcessing(newsockfd);

      if (rw < 0) break;
      if (rw == 0){
        strcpy(str, SendMessage);
	char ct[5];
	sprintf(ct ,"%d",N);
	strcat(str, ct);
      }

      if (rw > 0){
      printf(SendMessage);
      printf("\n");
      strcpy(str, SendMessage);
      }
      i+= rw;

      if (i > N)
      {
 	
	char messg[80];    
        if(winComp < win){
		sprintf(messg ,"\nScore = %d - %d (player 2 won)\n", winComp, win);
	}
   	else if(winComp > win){
   		sprintf(messg , "\nScore = %d - %d (player 1 won)\n", winComp, win);
	}
	else{
		strcpy(messg,"\nGames are Tied\n");
	}

	printf(messg);
	strcat(str, messg);
      }

     write(newsockfd,str, 511);
	
   }
	close(newsockfd);
	exit(0);
   return 0;
}

void child(){
	srand(time(NULL));
	int computer;
	computer = (rand ()%3) + 1;
	close(pd[0]); 
	write(pd[1], &computer, sizeof(computer));
	close(pd[1]);
	exit(0);
}

int DoServerProcessing(int newsockfd){
   int received_int = 0;
   int  player, computer;
   char buf[BUFSIZ];
   
	read(newsockfd, &received_int, sizeof(received_int));
	
	player = ntohl(received_int);
	
	if (player == 0)
	{
		printf("Player 2 connected\n");
 		SendMessage ="READY\n";
		return 0;
	}	
	   
   if ( pipe(pd) < 0 ) {
	error("can’t open pipe 1");
       return -1;
	}
   
   if (fork()==0){
			child();
			exit(0);
		}
	close(pd[1]);
   
   read(pd[0], &computer, sizeof(computer));
   
   if (player == computer) {
               SendMessage="Tie games\n";
	}
	if (player==1 && computer==2) {
		SendMessage = "Loss, paper covers rock \n";
		winComp++;
	}
	if (player==1 && computer==3) {
                SendMessage ="Win, rock breaks scissors \n";
		win++;
	}
	if (player==2 && computer==1) {
		SendMessage = "Win, paper covers rock\n";
		win++;
	}
	if (player==2 && computer==3) {
                SendMessage ="Loss, scissors cut paper\n";
                winComp++;
	}
	if (player==3 && computer==1) {
		SendMessage ="Loss, rock breaks scissors\n";
                winComp++;
	}
	if (player==3 && computer==2) {
                SendMessage ="Win, scissors cut paper\n";
		win++;
	}
	read(pd[0], buf, BUFSIZ);
	close(pd[0]);
       return 1;
}
