#include<Windows.h>
#include"ClientManager.h"
#include"Global.h"
pCLIENT_INFO AllocClient()
{
	pCLIENT_INFO CInfo = malloc(sizeof(CLIENT_INFO));
	memset(CInfo, 0, sizeof(CLIENT_INFO));
	CInfo->PackHeader[0].len = sizeof(BYTE);
	CInfo->PackHeader[0].buf = &(CInfo->PackID);

	CInfo->PackHeader[1].len = sizeof(unsigned int);
	CInfo->PackHeader[1].buf = &(CInfo->PackLen);

	CInfo->PackParseState = PARSE_WAITFOR_HEADER;//等待接受数据包头
	//剩下那个....等收到了头部再处理

	CInfo->Data = AllocVBuf();

	return CInfo;
}

BOOL FreeClient(pCLIENT_INFO CInfo)
{
	FreeVBuf(CInfo->Data);

	free(CInfo);
	return TRUE;
}

