#ifndef __CLIENT_APP_H__
#define __CLIENT_APP_H__

#include "ClientNet.h"
#include "../Protocol/Protocol.h"

typedef struct ClientApp ClientApp;

/*typedef enum{
	CLIENTNET_CONNECTION_FAILED,
	
} CAppErr;*/


ClientApp* CreateClientApp();

Respond RunClientApp(ClientApp* _clt);

void DestroyClientApp(ClientApp* _clt);


#endif /*__CLIENT_APP_H__*/
