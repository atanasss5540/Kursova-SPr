#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include<time.h>
#include<stdbool.h>

#define BUFF_SIZE 500

int main(int args, char *argv[])
{
	int random[10]; 
	srand(time(NULL));
	int i,j,a;
	char buffer[BUFF_SIZE] = {'\0'};
	char random1[500];
	int sockfd = 0;
	int bytes_read = 0;
	int ret_status = 0;
	int server_len = 0;

	struct sockaddr_un server_addr;

	//generate random numbers:
	for (i=0;i<10;i++)
	{
    		bool check; 
    		int n; 
    		do
    		{
    			n=rand()%20;
    			check=true;
	
    		for (j=0;j<i;j++)
        	if (n == random[j]) 
        	{
            		check=false; 
            		break; 
        	}
 	
    		} while (!check); 
   	random[i]=n++; 
	}

	//sort numbers
	for (i = 0; i < 10; ++i)
    	{
        	for (j = i + 1; j < 10; ++j)
        	{
            		if (random[i] > random[j])
            		{
                		a =  random[i];
                		random[i] = random[j];
                		random[j] = a;
            		}
        	}
    	}

	// Initialize socket struct to 0
	(void) memset((char* ) &server_addr, 0, sizeof(server_addr));
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
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		(void) printf("Creating socket...\n");
	}

	// Connecting to server socket.
	ret_status = connect(sockfd, (struct sockaddr *) & server_addr, server_len);
	if(ret_status < 0)
	{
		(void) printf("Connecting...\n");
	}

	//Get test 
	bytes_read = read(sockfd, buffer, BUFF_SIZE);
	if(bytes_read < 0)
	{
		(void) printf("Nothing received...\n");
	}
	(void) printf("Test : ");
	(void) printf("%s\n", buffer);
	(void) printf("Your Q are:\n");
    	for (i = 0; i < 10; i++)
        	printf("%d, ", random[i]+1);
	
	// Preparing messag to server.
	(void) printf("\nEnter name,FN and your ansers: ");
	fgets(buffer, BUFF_SIZE, stdin);

	
	int index = 0;
	for (i=0; i<10; i++)
   	index += sprintf(&random1[index], "%d, ", (random[i]+1));
	
	
	//concat random with buffer
	strcat(random1,buffer);
	
	// Sending message to server.
	write(sockfd, random1, strlen(random1));

	(void) close(sockfd);

	return 0;

}
