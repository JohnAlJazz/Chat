#ifndef _GNU_SOURCE
#define _GNU_SOURCE

#include <stdio.h>          /* printf(), snprintf() */
#include <stdlib.h>         /* strtol(), exit() */
#include <sys/types.h>
#include <sys/socket.h>     /* socket(), setsockopt(), bind(), recvfrom(), sendto() */
#include <errno.h>          /* perror() */
#include <netinet/in.h>     /* IPPROTO_IP, sockaddr_in, htons(), htonl() */
#include <arpa/inet.h>      /* inet_addr() */
#include <unistd.h>         /* fork(), sleep() */
#include <sys/utsname.h>    /* uname() */
#include <string.h>         /* memset() */

#define MAXLEN 4096
#define MAX_LENGTH_PID 20

int main(int argc, char* argv[])
{
	struct sockaddr_in sin;  /* mcast_group */
	struct ip_mreq mreq;
	struct utsname name;
    struct sockaddr_in from;
	unsigned char no = 0;
    unsigned int yes = 1;
	unsigned char ttl;
	int sock, ibind, isetsockopt;
	socklen_t len, sinLength = sizeof(sin);
	ssize_t readBytes;
	char buffer[MAXLEN + 1];
	char* dinBuffer;
	
	int pid;
	char c_pid[20];
	char* ip, *port, *userName;
	FILE* file;
	char readIp[20];
	char readPort[20];
	char readUserName[20];	
		
	if (argc != 2)
	{
        fprintf(stderr, "Usage: %s mcast_group port\n", argv[0]);
        perror("argc failed");
    }
	file = fopen(argv[1], "r");
	if(file == NULL)
	{
		perror("file open failed");
		return;
	}
	fgets(readIp, 20, file);
	fgets(readPort, 20, file);
	fgets(readUserName, 20, file);	
	fclose(file);
	
	/* process id */
	file = fopen("pIdListener.txt", "w");
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
 
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons((unsigned short int)strtol(readPort, NULL, 0));
    sin.sin_addr.s_addr = inet_addr(readIp);
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror ("socket failed");
        sleep(10);
		return 0;
    }
	
	isetsockopt = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (isetsockopt < 0)
    {
        perror("setsockopt failed");
        close(sock);
        sleep(10);
		return 0;
    }
	
	ibind = bind(sock, (struct sockaddr*)&sin, sizeof(sin));
    if (ibind < 0)
    {
        perror ("bind failed");
        close(sock);
        sleep(100);
		return 0;
    }

    /* Preparations for using Multicast */ 
	mreq.imr_multiaddr = sin.sin_addr;
	mreq.imr_interface.s_addr = INADDR_ANY;

     /* Tell the kernel we want to join that multicast group */
	isetsockopt = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	if (isetsockopt < 0)
	{
		perror ("add_membership setsockopt failed");
		close(sock);
		sleep(100);
		return 0;
	}	

	while(1)
	{
		len = sizeof(from);
		readBytes = recvfrom(sock, buffer, MAXLEN, 0, (struct sockaddr*)&from, &len);
		
		if (readBytes < 0)
		{
			perror ("recv failed");
			close(sock);
			sleep(10);
			return 0;
		}		
			
		printf("\033[32m>>\033[0m");		
		printf(" %s\n", buffer);		
	}
	return 0;
}

#endif /* _GNU_SOURCE */