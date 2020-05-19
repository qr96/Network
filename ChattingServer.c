/*
Student ID : 20153206
Name : Kyuhan Lee
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 100

void error_handling(char *message);
void display();

#define MAXCLIENT (16)
int clienttcpSocket[MAXCLIENT];	// max 16 clients


int main(int argc, char **argv) {
	
  int tcpServ_sock;
  
  struct sockaddr_in tcpServer_addr;
  struct sockaddr_in tcpClient_addr;
  struct sockaddr_in newTcp_addr;

  // initialize the client socket descriptorst77
  for(int i = 0; i < MAXCLIENT; i++) {
  	clienttcpSocket[i] = -1;;	// max 16 clients
    
  }

  int slot;
  socklen_t clnt_len;
    
  fd_set reads, temps;
  int fd_max;

  
  char str[BUFSIZE];
  int option = 2;

  char *tcpport = NULL;

  if(argc != 2) {
    printf("Usage : %s <tcpport> \n", argv[0]);
    exit(1);
  }

  tcpport = argv[1];

  display();
	
  tcpServ_sock = socket(PF_INET, SOCK_STREAM, 0);
  if(tcpServ_sock == -1)
	  error_handling("socket() error");
  
  memset(&tcpServer_addr, 0 , sizeof(tcpServer_addr));
  tcpServer_addr.sin_family = AF_INET;
  tcpServer_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  tcpServer_addr.sin_port = htons(atoi(tcpport));

  setsockopt(tcpServ_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&option, sizeof(int));
  if( bind(tcpServ_sock, (struct sockaddr *) &tcpServer_addr, sizeof(tcpServer_addr)) == -1 )
	  error_handling("bind() error");

			
  if(listen(tcpServ_sock, 5)==-1)
    error_handling("listen() error");
  
  FD_ZERO(&reads);
  FD_SET(tcpServ_sock, &reads);
  fd_max = tcpServ_sock;
  
  slot = 0;
  while(1) {
    int nfound = 0;
    int addrlen = 0;
    int new_sock;
    int bytesread;

    temps = reads;
    
    nfound = select(fd_max+1, &temps, 0, 0, NULL);
	for(int i=0;i<MAXCLIENT;i++){
		if(clienttcpSocket[i]==-1){
			slot=i;
			break;
		}
	}
	if(FD_ISSET(tcpServ_sock, &temps) && slot<MAXCLIENT) {
		addrlen = sizeof(tcpClient_addr);
		new_sock = accept(tcpServ_sock,(struct sockaddr *)&tcpClient_addr, &addrlen);
		if (new_sock < 0) {
			perror("accept");
			exit(1);
		}
		clienttcpSocket[slot]=new_sock;
		printf("connection from host %s, port %d, socket %d, slot %d\n",
		inet_ntoa(tcpClient_addr.sin_addr), ntohs(tcpClient_addr.sin_port),new_sock,slot);
		FD_SET(clienttcpSocket[slot], &reads);
		if (new_sock > fd_max) fd_max = new_sock;	

  		FD_CLR(tcpServ_sock, &temps);

	// NEED TO IMPLEMENT HERE



	} 
	else {
		for(int i = 0; i < MAXCLIENT; i++) {
			if (FD_ISSET(clienttcpSocket[i], &temps)) {
			//FD_CLR(clienttcpSocket[i], &temps);

				bytesread = read(clienttcpSocket[i], str, sizeof str - 1);
				if (bytesread<0) {
					perror("read");
					/* fall through */
				}
				if (bytesread<=0) {
					printf("Connection Closed %d\n",clienttcpSocket[i]);
					FD_CLR(clienttcpSocket[i], &reads);
					if (close(clienttcpSocket[i])) perror("close");
					clienttcpSocket[i]=-1;
					continue;
				}
				str[bytesread] = '\0';
				for(int j = 0; j < MAXCLIENT; j++) {
					if(i!=j && clienttcpSocket[j] != -1){
						if (write(clienttcpSocket[j], str, bytesread)!=bytesread)
						perror("echo");
					}
					
				}

			}
		}
	}

  }//while End
}//main End

void display() {
	printf("Student ID : 20153206 \n");
	printf("Name : Kyuhan Lee \n");
}

void error_handling(char *message) {
  fputs(message, stderr);
  fputc('\n', stderr);
  perror("hw3 error");
  exit(1);
}


