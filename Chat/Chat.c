#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "Chat.h"

#define CMD_LENGTH 150

void OpenWindows(char* _ip, char* _port, char* _userName, char* _groupName)
{
	char fileName[] = "Parameters.txt";
	char senderCmd[CMD_LENGTH];
	char listenerCmd[CMD_LENGTH];	

	FILE* file = fopen(fileName, "w");	

	if(!file)
	{
		perror("couldn't open Parameters.txt");
		return;
	}	

	fprintf(file, "%s\n", _ip);
	fprintf(file, "%s\n", _port);
	fprintf(file, "%s\n", _userName);	
	fclose(file);	
	
	sprintf(senderCmd,
		"gnome-terminal --title\
		%s --geometry=10x3\
	 	-- bash -c './../Parameters/SenderMain.out Parameters.txt'", _groupName);
	sprintf(listenerCmd,
		"gnome-terminal --title\
		%s --geometry=20x10\
		-- bash -c './../Parameters/ListenerMain.out Parameters.txt'", _groupName);
		
	system(senderCmd);
	system(listenerCmd);
}

void CloseWindows(char* _proccesse1, char* _proccesse2)
{
	int pId1, pId2;
		
	pId1 = atoi(_proccesse1);
	pId2 = atoi(_proccesse2);
		
	kill(pId1, SIGTERM);	
	kill(pId2, SIGTERM);
}