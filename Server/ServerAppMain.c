#include "ServerApp.h"

int main(void)
{	
	Server* ptrServer = Create(3000);
	Run(ptrServer);
	Destroy(ptrServer);
	
	return 0;
}
