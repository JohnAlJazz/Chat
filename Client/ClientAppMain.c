#include "ClientApp.h"
#include "ClientNet.h"
#include "../Protocol/Protocol.h"
#include "UIChat.h"

int main()
{
	ClientApp* clientPtr = CreateClientApp();
	RunClientApp(clientPtr);
	DestroyClientApp(clientPtr);
	
	return 0;
}