#include "inet.h"
#include <stdbool.h>
#define BUFSIZE 1024

main()
{
	int sockfd, new_sockfd, clilen;
	char buffer[BUFSIZE+1];
	struct sockaddr_in serv_addr, cli_addr;
	int bytesreceived = 0;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	  perror("Server: socket() error\n");
	  exit(1);
	}

	FILE *fp;
	char returndata [64];	
	char *ipaddr;
	char x[1000];
	fp = popen("/sbin/ifconfig eth0", "r");
	while(fgets(returndata,64,fp)!=NULL)
	{
		strcat(x,returndata);
	}
	ipaddr = strtok(x," ");
	ipaddr = strtok(NULL," ");
	ipaddr = strtok(NULL," ");
	ipaddr = strtok(NULL," ");
	ipaddr = strtok(NULL," ");
	ipaddr = strtok(NULL," ");
	ipaddr = strtok(NULL," ");
	ipaddr = strtok(NULL," addr:");
	
	printf("Please connect to: %s\n",ipaddr);

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ipaddr);
	serv_addr.sin_port = htons(SERV_TCP_PORT);

	pclose(fp);

	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	perror("Server: bind() error\n");
	printf("\nWaiting for connection... [bind]\n");

	
	char *uname; 
	uname=(char *)malloc(10*sizeof(char)); 
	uname=getlogin(); 

	
	char servdir[30]; 
	strcpy(servdir, "/home/"); 
	strcat(servdir, uname); 
	strcat(servdir, "/tcpserver/"); 

	
	struct stat s; 
	if(stat(servdir, &s) == -1)
	{ 
		mkdir(servdir, 0700); 
	}

	listen(sockfd, 5);
	int paip[2];
	if(pipe(paip)==-1)
	{
		perror("Create pipe has failed.");
		exit(1);
	} 

	for(;;)
	{
		clilen = sizeof(cli_addr);
		new_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

	
		char *cliaddr = inet_ntoa(cli_addr.sin_addr);
		char *eachcliaddr; 
		write(paip[1], &cliaddr, sizeof(cliaddr));
	
	

	if(fork() == 0)
	{
		close(sockfd);
		strcpy(buffer,"--[FILE REPOSITORY SERVER]--\nMenu\n1. Make directory\n2. Delete directory (all subfolders will be deleted)\n3. Send file\n4. Download file\n[ /q to quit] : ");
	  	send(new_sockfd, buffer, BUFSIZE, 0);


		if(new_sockfd > 0)
		
		  read(paip[0],&eachcliaddr,sizeof(eachcliaddr));
		  printf("\nClient with IP address %s is connected.\n", eachcliaddr);
	  
		do{
		  
			recv(new_sockfd, buffer, BUFSIZE, 0);
		 	if(!strcmp(buffer, "1"))
			{
				bzero( buffer, sizeof(buffer));
				strcat(buffer,"Directory created. [/q to quit]");
			}

			if(!strcmp(buffer, "2"))
			{
				bzero( buffer, sizeof(buffer));
				strcat(buffer,"Directory deleted. [/q to quit]");
			}


			if(!strcmp(buffer, "3"))
			{

				bzero( buffer, sizeof(buffer));
				recv(new_sockfd, buffer, BUFSIZE, 0);

				char filename[30];
				strcpy(filename, "/home/"); 
				strcat(filename, uname); 
				strcat(filename, "/tcpserver/");
				strcat(filename, buffer);


				FILE *fp;
			 	fp = fopen(filename, "ab"); 
				bzero( buffer, sizeof(buffer));
				bytesreceived = recv(new_sockfd, buffer, BUFSIZE, 0);
				fwrite(buffer,1,bytesreceived,fp);
				fclose(fp);

				bzero( buffer, sizeof(buffer));
				strcat(buffer,"Sucessfully sent to server. [/q to quit]");
			}
	
			if(!strcmp(buffer, "4"))
			{	
				bzero( buffer, sizeof(buffer));
				strcat(buffer,"[List of Files in the Server Directory]\nPlease select a file from the list to be download:\n\n");

				DIR *dir;
				struct dirent *ent;

				char dirname[30];
		 		strcpy(dirname, "/home/"); 
	  			strcat(dirname, uname); 
	  			strcat(dirname, "/tcpserver/");

				if ((dir = opendir (dirname)) != NULL) 
				{

					
		 			while ((ent = readdir (dir)) != NULL) 
					{

						strcat(buffer, ent->d_name);
						strcat(buffer, "\n");
		 			}
					closedir (dir);
					send(new_sockfd, buffer, BUFSIZE, 0);
				 
				}
				else 
				{

					
		 		 perror ("Directory does not exist.");
					return EXIT_FAILURE;
				}

			bool exist = true; 

			do{

			      
				bzero( buffer, sizeof(buffer));
				recv(new_sockfd, buffer, BUFSIZE, 0);

				char filename[30];
				strcpy(filename, "/home/"); 
				strcat(filename, uname); 
				strcat(filename, "/tcpserver/");
				strcat(filename, buffer);
		

				FILE *fp = fopen(filename, "r");
				if(fp==NULL)
				{
			    		strcpy(buffer," No such file in server. Enter correct filename with extension.");
					send(new_sockfd, buffer, BUFSIZE, 0);  
					exist = false; 
					 
				}  

				if(exist == true )
				{
					bzero( buffer, sizeof(buffer));
					int nread = fread(buffer,1,256,fp);
					send(new_sockfd, buffer, nread, 0);
				}

				bzero(buffer, sizeof(buffer));
				strcpy(buffer,"Download completed. [/q to quit]");

			}while(exist == false);
		}

			send(new_sockfd, buffer, BUFSIZE, 0);
		}while(strcmp(buffer, "/q"));

		printf("\nClient disconnected. IP Address: %s \n", eachcliaddr);
		exit(0);
	}
	close(new_sockfd);
	}
	close(sockfd);
}
