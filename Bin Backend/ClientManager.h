#pragma once
#include<Windows.h>
#include"Global.h"
#include"VBuffer.h"


typedef struct __tagClientInfo
{
	int ListIndex;//���ڻ����б��е�λ��
	SOCKET ClientSock;//����ͻ��˵�Socket

	int ClientID;
	PWCHAR ClientDescription;//�������Ͱ����������

	int PackParseState;
	WSABUF PackHeader[2];//���ݰ�ͷ
	BYTE PackID;
	unsigned int PackLen;

	WSABUF PackBody;
	pVBUF Data;


	SRWLOCK WSASendLock;//һ���ͻ���һ��ֻ����һ�����ݰ�����
	
	//LONG IsSending;//����û��ʹ��SRWLOCK,(���߳�ʹ����Դ) ֻ����ԭ����
	WSABUF PackSend[3];
	pVBUF SendData;//�������ݻ�����
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
//��������������������Ҫ���������SRW��
int ClientCount;
pCLIENT_INFO CInfoList[MAX_CLIENT];