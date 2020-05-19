// Name : KYUHAN LEE

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

#define PROMPT() {printf("\n> ");fflush(stdout);}
#define GETCMD "wget"
#define QUITCMD "quit"


int main(int argc, char *argv[]) {
	int socktoserver = -1;
	char buf[BUFSIZ];

	char fname[BUFSIZ];	

	printf("Name : Kyuhan Lee\n");

	PROMPT(); 

	for (;;) {
		if (!fgets(buf, BUFSIZ - 1, stdin)) {
			if (ferror(stdin)) {
				perror("stdin");
				exit(1);
			}
			exit(0);
		}

		char *cmd = strtok(buf, " \t\n\r");

		if((cmd == NULL) || (strcmp(cmd, "") == 0)) {
			PROMPT(); 
			continue;
		} else if(strcasecmp(cmd, QUITCMD) == 0) {
				exit(0);
		}

		if(!strcasecmp(cmd, GETCMD) == 0) {
			printf("Wrong command %s\n", cmd);
			PROMPT(); 
			continue;
		}

		// connect to a server
		char *hostname = strtok(NULL, " \t\n\r");
		char *pnum = strtok(NULL, " ");
		char *filename = strtok(NULL, " \t\n\r");

		// just for Debugging, print the URL
		//printf("%s:%s%s\n", hostname, pnum, filename);


		// NEED TO IMPLEMENT HERE
		
		char *ftmp = strrchr(filename,'/')+1;
		strcpy(fname,ftmp);

		struct hostent *hostp;
		struct sockaddr_in server;
		int sock;

		if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
			perror("socket");
			exit(1);
		}

		if ((hostp = gethostbyname(hostname)) == 0) {
			fprintf(stderr,"%s: unknown host\n",hostname);
			exit(1);
		}
		
		memset((void *) &server, 0, sizeof (server));
		server.sin_family = AF_INET;
		memcpy((void *) &server.sin_addr, hostp->h_addr, hostp->h_length);
		server.sin_port = htons((u_short)atoi(pnum));
		
		if (connect(sock, (struct sockaddr *)&server, sizeof (server)) < 0) {
			(void) close(sock);
			fprintf(stderr, "connect");
			exit(1);
		}
		
		char tmpf[40];
		char tmph[40];
		strcpy(tmpf,filename);
		strcpy(tmph,hostname);		
		
		
		strcpy(buf,"GET ");
		strcat(buf, tmpf);
		strcat(buf, " HTTP/1.0\r\n");
		strcat(buf, "HOST: ");
		strcat(buf, tmph);
		strcat(buf,"\r\n");
		strcat(buf,"User-Agent: HW1/1.0\r\n");
		strcat(buf,"Name: Kyuhan Lee\r\n");
		strcat(buf,"Connection: close\r\n");
		strcat(buf,"\r\n");

		FILE *fp;
		char *dt;
		fp = fopen(fname,"w");

		if (write(sock, buf, strlen(buf)) < 0) {
			perror("write");
			exit(1);
		}
		
		
		int recvn;
		int size;
		int cdown=0;
		int pdown=0;
		char tmp[50];
		char *pt1, *pt2;
		while((recvn=recv(sock, buf, sizeof buf,0))>0){
			if(dt==NULL&&recvn!=0){
				dt=strstr(buf,"\r\n\r\n")+4;
				pt1 = strstr(buf,"Content-Length");
				strcpy(tmp,strtok(pt1,"\r\n"));
				size=atoi(tmp+16);
				printf("Total Size %d bytes\n",size);
				fwrite(dt,sizeof(char),recvn-(dt-buf),fp);
			}
			else{
				fwrite(buf,sizeof(char),recvn,fp);
			}
			cdown+=recvn;
			
			if(size!=0 && cdown-pdown>=size/10 && cdown-pdown<size){
				printf("Current Downloading %d/%d (bytes) %d%\n",cdown,size,cdown*100/size);
				pdown=cdown;
			}
			else if(cdown-pdown>size){
				printf("Current Downloading %d/%d (bytes) 100%\n",size,size);
			}
			//printf("test:%d\n",recvn);
		}
		
		printf("Download Complete: %s, %d/%d",fname,size,size);
		fclose(fp);
		close(sock);
	}
} 




