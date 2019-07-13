#pragma once
#include<Windows.h>
#include"Global.h"
#include"VBuffer.h"


typedef struct __tagClientInfo
{
	int ListIndex;//用于回溯列表中的位置
	SOCKET ClientSock;//这个客户端的Socket

	int ClientID;
	PWCHAR ClientDescription;//这个垃圾桶的文字描述

	int PackParseState;
	WSABUF PackHeader[2];//数据包头
	BYTE PackID;
	unsigned int PackLen;

	WSABUF PackBody;
	pVBUF Data;


	SRWLOCK WSASendLock;//一个客户端一次只能有一个数据包发送
	
	//LONG IsSending;//由于没法使用SRWLOCK,(跨线程使用资源) 只能用原子锁
	WSABUF PackSend[3];
	pVBUF SendData;//发送数据缓冲区
	BYTE SendPackID;
	unsigned int SendPackLen;


}CLIENT_INFO, * pCLIENT_INFO;


BOOL InitClientManager();
pCLIENT_INFO AllocClient();
BOOL FreeClient(pCLIENT_INFO CInfo);

#define PARSE_WAITFOR_HEADER 1
#define PARSE_WAITFOR_PACKBODY 2

#define MAX_CLIENT 1024


SRWLOCK CInfoListSRWLock;
//访问下面这两个变量需要先申请访问SRW锁
int ClientCount;
pCLIENT_INFO CInfoList[MAX_CLIENT];