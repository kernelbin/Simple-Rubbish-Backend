#pragma once
#include<Windows.h>
#include"Global.h"
#include"VBuffer.h"


typedef struct __tagClientInfo
{
	SOCKET ClientSock;//这个客户端的Socket

	int ClientID;
	PWCHAR ClientDescription;//这个垃圾桶的文字描述

	int PackParseState;
	WSABUF PackHeader[2];//数据包头
	BYTE PackID;
	unsigned int PackLen;

	WSABUF PackBody;
	pVBUF Data;
}CLIENT_INFO, * pCLIENT_INFO;


pCLIENT_INFO AllocClient();
BOOL FreeClient(pCLIENT_INFO CInfo);

#define PARSE_WAITFOR_HEADER 1
#define PARSE_WAITFOR_PACKBODY 2

#define MAX_CLIENT 1024

int ClientCount;
pCLIENT_INFO CInfoList[MAX_CLIENT];