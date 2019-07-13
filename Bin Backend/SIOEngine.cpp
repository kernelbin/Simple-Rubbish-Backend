#include<Windows.h>
#include<process.h>
#include"sio_client.h"
#include"Global.h"

using std::string;

sio::client h;


extern "C" BOOL SocketIOConnect()
{
	h.connect(SERVER_ADDR ":" SERVER_PORT);
	return TRUE;
}