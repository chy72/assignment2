#include "inet.h"
#define BUFSIZE 1024

main(int argc, char *argv[])
{
	int sockfd;
	char buffer[BUFSIZE+1];
	int bytesreceived = 0;
	struct sockaddr_in serv_addr;
	char create[20];
	char delete[20];
	static struct sigaction act;

	void catchin(int);

	act.sa_handler = catchin;
	sigfillset(&(act.sa_mask));

	sigaction(SIGINT, &act, (void *) 0);


	if(argc <= 1)
	{
		printf("How to use: %s remoteIPaddress [For example: ./client 127.0.0.1]\n", argv[0]); 
		exit(1); 
	}

	bzero( (char*) &serv_addr, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET ;
	serv_addr.sin_port = htons (SERV_TCP_PORT);
	inet_pton (AF_INET, argv[1], &serv_addr.sin_addr);

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0) 
	{
		perror("Client: socket() error \n");
		exit(1); 
	}

	if(connect (sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0 ) 
	{
		perror("Client: connect() error\n");
		exit(1);
	}

	
	char *uname; 
	uname=(char *)malloc(10*sizeof(char)); 
	uname=getlogin();

	 
	char clidir[30]; 
	strcpy(clidir, "/home/"); 
	strcat(clidir, uname); 
	strcat(clidir, "/tcpclient/"); 

	
	struct stat s; 
	if(stat(clidir, &s) == -1)
	{ 
		mkdir(clidir, 0700); 
	}

	do{
		bzero( buffer, sizeof(buffer));
		recv(sockfd, buffer, BUFSIZE, 0);
		printf("%s\n", buffer); 
		gets(buffer);
		send(sockfd,buffer, BUFSIZE, 0);

		if(!strcmp(buffer, "1"))
		{
			printf("Enter name for creating directory: ");
			scanf("%s", create);

			
			char createdir[30]; 
			strcpy(createdir, "/home/"); 
			strcat(createdir, uname); 
			strcat(createdir, "/"); 
			strcat(createdir, create);

			 
			struct stat s; 
			if(stat(createdir, &s) == -1){ 
			mkdir(createdir, 0700); } 
		}


		else if(!strcmp(buffer, "2"))
		{
			printf("Enter name for deleting directory: ");
			scanf("%s", delete);

			
			char deletedir[30]; 
			strcpy(deletedir, "/home/"); 
			strcat(deletedir, uname); 
			strcat(deletedir, "/"); 
			strcat(deletedir, delete);

			
			char selsubdir[50];
			strcpy(selsubdir, "exec rm -r ");
			strcat(selsubdir, "/home/"); 
			strcat(selsubdir, uname); 
			strcat(selsubdir, "/"); 
			strcat(selsubdir, delete);
			strcat(selsubdir, "/*"); 

			
			struct stat s; 
			if(stat(deletedir, &s) != -1)
			{
				system(selsubdir);
				rmdir(deletedir); 
			} 
		
		else if(!strcmp(buffer, "3"))
		{
			DIR *dir;
			struct dirent *ent;

			char dirname[30];
		   	strcpy(dirname, "/home/"); 
			strcat(dirname, uname); 
			strcat(dirname, "/tcpclient/");

			if ((dir = opendir (dirname)) != NULL) 
			{
		
				printf("\n[File list]\n");
		  		
		  		while ((ent = readdir (dir)) != NULL) {

				printf("%s\n", ent->d_name);    }

		 		closedir (dir);
			}

			printf("\nEnter filename with extension:\n");
	
			bzero( buffer, sizeof(buffer));
			gets(buffer);
			send(sockfd,buffer, BUFSIZE, 0);

			char filename[30];
			strcpy(filename, "/home/"); 
			strcat(filename, uname); 
			strcat(filename, "/tcpclient/");
				
			strcat(filename, buffer);

			FILE *fp;
			fp = fopen(filename, "r"); 

			bzero( buffer, sizeof(buffer));
			int nread = fread(buffer,1,256,fp);
			send(sockfd, buffer, nread, 0);	
		}

		else if(!strcmp(buffer, "4"))
		{
			bzero( buffer, sizeof(buffer));
			recv(sockfd, buffer, BUFSIZE, 0);
			printf("\n%s\n", buffer); 
			gets(buffer);
			send(sockfd,buffer, BUFSIZE, 0);
	
		   	char filename[30];
			strcpy(filename, "/home/"); 
			strcat(filename, uname); 
			strcat(filename, "/tcpclient/");
			strcat(filename, buffer);
			
			FILE *fp;
			fp = fopen(filename, "ab"); 

			if(NULL == fp)
			{
				printf("open file failed");
	
			}
		
			bzero( buffer, sizeof(buffer));
		
			bytesreceived = recv(sockfd, buffer, BUFSIZE, 0);
			fwrite(buffer,1,bytesreceived,fp);
			close(sockfd);
		   
		}


		


		
		}

	}while (strcmp(buffer, "/q"));
	
	close(sockfd);
}

	void catchin(int signo)
	{
		printf("\n[ Interrupt signal ignored.]\n");
	}
