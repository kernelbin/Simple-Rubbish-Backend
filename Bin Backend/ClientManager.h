#pragma once
#include<Windows.h>
#include"Global.h"
#include"VBuffer.h"


typedef struct __tagClientInfo
{
	int ClientID;
	PWCHAR ClientDescription;//�������Ͱ����������

	int PackParseState;
	WSABUF PackHeader[2];//���ݰ�ͷ
	BYTE PackID;
	unsigned int PackLen;

	pVBUF Data;
}CLIENT_INFO, * pCLIENT_INFO;


pCLIENT_INFO AllocClient();
BOOL FreeClient(pCLIENT_INFO CInfo);

#define PARSE_WAITFOR_HEADER 1
#define PARSE_WAITFOR_PACKBODY 2

