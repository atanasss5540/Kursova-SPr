#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>


#define BUFF_SIZE 500
#define CLIENT_NUM (int) 100

char nametest[20];
char nameansw[20];
void *connection_handler(void *);

int main(int args, char *argv[])
{
	
	int sockfd = 0;
	int i=0;
	int client_sock = 0;
	int ret_status = 0;
	int *new_sock;
	int server_len = 0;
	char symbol;

	socklen_t client_len;

	struct sockaddr_un server_addr, client_addr;

	//entering files names for read and write
	printf("\n Enter test name file: ");
	scanf("%s",nametest);
	printf("\n Enter test answers file: ");
	scanf("%s",nameansw);
	
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("Creating socket...\n");
	}

	// Initialize server to 0
	(void) memset((char*) &server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	if(1 == args)
	{
		(void) printf("Not enough arguments!\n");
	}
	else if(2 == args)
	{
		(void) strcpy(server_addr.sun_path, argv[1]);
	}
	else
	{
		(void) printf("Too many arguments\n");
	}
	server_len = sizeof(server_addr);
	ret_status = bind(sockfd, (struct sockaddr *) & server_addr, server_len);
	if(ret_status < 0)
	{
		(void) printf("Binding...\n");
	}

	// Listening for clients, Define ckient number in backlog.
	(void) listen(sockfd, CLIENT_NUM);

	// Accepting new client
	client_len = sizeof(client_addr);
	for(i=0;i<CLIENT_NUM;i++)
	{
		client_sock = accept(sockfd, (struct  sockaddr *) &client_addr, &client_len);
		if(client_sock < 0)
		{
			(void) printf("Accepting...\n");
		}

		pthread_t sniffer_thread;
        	new_sock = malloc(1);
        	*new_sock = client_sock;
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        	{
            		perror("could not create thread");
            		return 1;
        	}
        }
     	if (client_sock < 0)
    	{
        	perror("accept failed");
        	return 1;
    	}
	return 0;

}
	//handler
void *connection_handler(void *sockfd){
	char symbol;
	int client_sock = 0;
	int sock=*(int*)sockfd;
 	char source[1000000];
	char buffer[5000];
	int bytes_read = 0;

	//concat test name file
	strcat(nametest,".txt");

	//read file
	FILE *fp = fopen(nametest, "r");
	if(fp != NULL)
	{
		while((symbol = getc(fp)) != EOF)
		{
		strcat(source, &symbol);
		}
	fclose(fp);
	}
	
	//send test
	(void)  write(sock, source, strlen(source));
		
	//read ansers
	bytes_read = read(sock, buffer, sizeof(buffer));
	if(bytes_read < 0)
	{
		(void) printf("Nothing received...\n");
	}
	int size=0;
	 while(buffer[size]!='\0')
	{
		size++;
	}

	//concat answers name file
	strcat(nameansw,".txt");

	//write file
	FILE *fp1 = fopen(nameansw, "a+");
	fwrite(buffer,1,size,fp1);
	fclose(fp1);
	(void) printf("Answers save !!!\n");

	(void) free(sockfd);
	return 0;
}
