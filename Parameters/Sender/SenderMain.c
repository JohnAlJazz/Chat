#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH_PID 20

int main(int argc, char* argv[])
{
	char processId[20];
	struct sockaddr_in CSock;  /* groupSock */
    struct in_addr localInterface;
    int sock, isetsockopt;
    ssize_t sendBytes;
    char bufferTemp[4000]/* = "Multicast test message lol!"*/;
    char buffer[4096];
    int buflen = sizeof(buffer);
    char* dinBuffer;
    
    int pid;
    char c_pid[20];
	char* ip, *port;
	FILE* file;
	char temp1[20];
	char temp2[20];
	char temp3[20];
	file = fopen(argv[1], "r");
	if(file == NULL)
	{
		perror("file open failed");
		sleep(10);
		return;
	}
	fgets(temp1, 20, file);
	fgets(temp2, 20, file);
	fgets(temp3, 20, file);
	fclose(file);
	
	/* process id */
	file = fopen("pIdSender.txt", "w");
	if(file == NULL)
	{
		perror("file open failed");
		sleep(10);
		return;
	}
	pid = getpid();
	snprintf(c_pid, MAX_LENGTH_PID, "%d", pid);
	fprintf(file,"%s\n", c_pid);
	fclose(file);

    /* Create a datagram socket on which to send. */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
      perror("Opening datagram socket error");
      return 0;
    }
    
    memset((char*) &CSock, 0, sizeof(CSock));
    CSock.sin_family = AF_INET;
    CSock.sin_addr.s_addr = inet_addr(temp1);
    CSock.sin_port = htons((unsigned short int)strtol(temp2, NULL, 0));

	while(1)
	{
		printf("Type your message:\n");
		scanf("%[^\n]%*c", bufferTemp);
		strcpy(buffer, temp3);
		strcat(buffer, bufferTemp);
		sendBytes = sendto(sock, buffer, buflen, 0, (struct sockaddr*)&CSock, sizeof(CSock));
    	if(sendBytes < 0)
		{
			perror("Sending datagram message error");
			close(sock);
			sleep(10);
   		  	return 0;
		}    	
	}
	return 0;
}
