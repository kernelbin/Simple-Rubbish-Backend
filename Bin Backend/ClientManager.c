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

	//ʣ���Ǹ�....���յ���ͷ���ٴ���
	return CInfo;
}

BOOL FreeClient(pCLIENT_INFO CInfo)
{
	free(CInfo);
	return TRUE;
}

