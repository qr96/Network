// Student ID : 20153206
// Name : Kyuhan Lee

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


int main(int argc, char *argv[]) {
	struct sockaddr_in server, remote;
	int request_sock, new_sock;
	int bytesread;
	socklen_t addrlen;
	char buf[BUFSIZ];

	if (argc != 2) {
		(void) fprintf(stderr,"usage: %s portnum \n",argv[0]);
		exit(1);
	}

	int portnum = atoi(argv[1]);

	if ((request_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		exit(1);
	}
	
	printf("Student ID : 20153206\n");
	printf("Name : Kyuhan Lee\n");

	// Create a Server Socket

	memset((void *) &server, 0, sizeof (server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons((u_short)atoi(argv[1]));

	if (bind(request_sock, (struct sockaddr *)&server, sizeof (server)) < 0) {
		perror("bind");
		exit(1);
	}
	if (listen(request_sock, SOMAXCONN) < 0) {
		perror("listen");
		exit(1);
	}

	while(1) {
		/* a new connection is available on the connetion socket */
		
		int sent = 0;
		char fsize[20];
		long int size=0;

		addrlen = sizeof(remote);
		new_sock = accept(request_sock,
			(struct sockaddr *)&remote, &addrlen);
		if (new_sock < 0) {
			perror("accept");
			exit(1);
		}
		printf("Connection : Host IP %s, Port %d, socket %d\n",
			inet_ntoa(remote.sin_addr), ntohs(remote.sin_port), new_sock);

		//for (;;) {
			bytesread = read(new_sock, buf, sizeof (buf) - 1);
			if (bytesread<=0) {
				//printf("finish %d %s\n",sent,fsize);
				if (close(new_sock)) 
					perror("close");
				break;
			}
			buf[bytesread] = '\0';

			printf("%s",buf);
			char *nptr = strtok(buf,"\r\n")+4;
			nptr = strtok(nptr," ")+1;
			char fname[30];//file name
			strcpy(fname,nptr);
			char buf2[BUFSIZ]="";//send buffer
			char fbuf[BUFSIZ]="";//file buffer
			int sendBytes=0;
			int totalBytes=0;

			memset(buf2,0,BUFSIZ);
			FILE *fp = fopen(fname,"r");
			if(fp <= 0){
				printf("Server Error : No such file ./%s!\n",fname);
				strcpy(buf2,"HTTP/1.0 404 NOT FOUND\r\n");
				strcat(buf2,"Connection: close\r\n");
				strcat(buf2,"ID: 20153206\r\n");
				strcat(buf2,"Name: Kyuhan Lee\r\n");
				strcat(buf2,"Content-Length: 0\r\n");
				strcat(buf2,"Content-Type: text/html\r\n");
				strcat(buf2,"\r\n");
				write(new_sock, buf2, strlen(buf2));
				//printf("%ld\n",sizeof(buf2));
			}
			else{
				
				fseek(fp,0,SEEK_END);
				size=ftell(fp);
				fseek(fp,0,SEEK_SET);
				sprintf(fsize,"%ld",size);
				
				strcpy(buf2,"HTTP/1.0 200 OK\r\n");
				strcat(buf2,"Connection: close\r\n");
				strcat(buf2,"ID: 20153206\r\n");
				strcat(buf2,"Name: Kyuhan Lee\r\n");
				strcat(buf2,"Content-Length: ");
				strcat(buf2,fsize);
				strcat(buf2,"\r\n");
				strcat(buf2,"Content-Type: text/html\r\n");
				strcat(buf2,"\r\n");
				send(new_sock, buf2, strlen(buf2),0);
				//printf("%s\n",buf2);
				
				memset(buf2,0,BUFSIZ);
				while((sendBytes=fread(buf2,sizeof(char),sizeof(buf2),fp)) > 0){
					send(new_sock,buf2,sendBytes,0);
					sent += sendBytes;
					//printf("%s\n",buf2);
					memset(buf2,0,BUFSIZ);
				}
				printf("finish %d %ld\n",sent,size);
				
				fclose(fp);
				sent=0;
			}
			
			
			
		//}
		//printf("%d %s\n",sndn/8,fsize);

	}
} /* main - hw2.c */


