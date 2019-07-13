#include<Windows.h>
#include"ClientManager.h"
#include"Global.h"

BOOL InitClientManager()
{
	ClientCount = 0;
	InitializeSRWLock(&CInfoListSRWLock);
	return TRUE;
}
pCLIENT_INFO AllocClient()
{
	//访问SRW锁，加进去然后申请内存，然后就可以退出SRW锁进行无关紧要的初始化
	AcquireSRWLockExclusive(&CInfoListSRWLock);
	if (ClientCount >= MAX_CLIENT)
	{
		ReleaseSRWLockExclusive(&CInfoListSRWLock);
		return 0;//不成
	}
	pCLIENT_INFO CInfo = malloc(sizeof(CLIENT_INFO));
	CInfoList[ClientCount] = CInfo;
	CInfo->ListIndex = ClientCount++;

	ReleaseSRWLockExclusive(&CInfoListSRWLock);
	//分配成了，初始化其他信息

	memset(CInfo, 0, sizeof(CLIENT_INFO));
	CInfo->PackHeader[0].len = sizeof(BYTE);
	CInfo->PackHeader[0].buf = &(CInfo->PackID);

	CInfo->PackHeader[1].len = sizeof(unsigned int);
	CInfo->PackHeader[1].buf = &(CInfo->PackLen);

	CInfo->PackParseState = PARSE_WAITFOR_HEADER;//等待接受数据包头
	//剩下那个....等收到了头部再处理

	CInfo->Data = AllocVBuf();

	InitializeSRWLock(&CInfo->WSASendLock);
	//CInfo->IsSending = 0;
	CInfo->SendData = AllocVBuf();
	//
	return CInfo;
}

BOOL FreeClient(pCLIENT_INFO CInfo)
{
	FreeVBuf(CInfo->Data);
	FreeVBuf(CInfo->SendData);
	//从表中删除
	AcquireSRWLockExclusive(&CInfoListSRWLock);
	CInfoList[CInfo->ListIndex] = CInfoList[--ClientCount];
	CInfoList[CInfo->ListIndex]->ListIndex = CInfo->ListIndex;
	ReleaseSRWLockExclusive(&CInfoListSRWLock);

	free(CInfo);
	return TRUE;
}

