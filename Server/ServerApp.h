#ifndef __SERVER_APP_H__
#define __SERVER_APP_H__

#include "ServerNet.h"

typedef struct AppData AppData;

typedef enum ServerAppErr
{
    SERVER_APP_SUCCESS,
    SERVER_APP_ADD_USER_FAIL
}ServerAppErr;


Server* Create(int _port);

void Run(Server* _srv);

void Destroy(Server* _srv);

#endif /* __SERVER_APP_H__ */
