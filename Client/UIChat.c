#include <stdio.h>		/*printf*/
#include <stdlib.h>		/*atoi*/
#include <string.h>

#include "UIChat.h"

static int CleanInput()
{
	while (getchar() != '\n');	
	return 1;
}

void PrintMainScreen(int* _tag, char* _name, char* _password)
{
	int tag;
	char c;

	system("clear");
	printf("\nWelcome to J-chat\n\nWhat would you like to do?\n\n1. Register\n\n2. Log in\n\n3. Exit\n\n");	
	while((scanf("%d%c", &tag, &c) != 2 || c != '\n') && CleanInput() || tag < 1 || tag > 3)
	{
		printf("\nPlease enter only 1 2 or 3:\n");
	}	
	if(tag != 3)
	{
		printf("\nPlease enter your name and press Enter:\n");
		scanf("%s", _name);
		printf("\nPlease enter a password and press Enter:\n");
		scanf("%s", _password);
	}	
	/* else 
	{
		strncpy(_name, "NAME", 10);
	} */	
	*_tag = tag;
}

void PrintGroupScreen(int* _tag, char* _groupName)
{
	int tag;
	char c;

	system("clear");
	printf("\n1. Join group\n\n2. Create group\n\n3. Leave group\n\n4. Log out\n\n");	
	while((scanf("%d%c", &tag, &c) != 2 || c != '\n') && CleanInput() || tag < 1 || tag > 4)
	{
		printf("\nPlease enter only 1 2 3 or 4:\n");
	}	
	if(tag != 4)
	{
		printf("\nPlease enter the group name and press Enter:\n");
		/* scanf reads until whitespaces (including space...), therefore => %[^\n] */
		scanf("%[^\n]", _groupName);				
	
	}	
	*_tag = tag;
}

void PrintMessage(char* _message)
{
	printf("%s\n",_message);
}


void EnterToContinue()
{
	getchar();
	printf("\nPlease press Enter to continue\n");
	while(getchar() != '\n'){}
}