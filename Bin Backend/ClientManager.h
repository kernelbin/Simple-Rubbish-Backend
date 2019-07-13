#pragma once
#include<Windows.h>
#include"Global.h"
#include"VBuffer.h"


typedef struct __tagClientInfo
{
	SOCKET ClientSock;//����ͻ��˵�Socket

	int ClientID;
	PWCHAR ClientDescription;//�������Ͱ����������

	int PackParseState;
	WSABUF PackHeader[2];//���ݰ�ͷ
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