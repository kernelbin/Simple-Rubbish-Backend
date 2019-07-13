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
	//����SRW�����ӽ�ȥȻ�������ڴ棬Ȼ��Ϳ����˳�SRW�������޹ؽ�Ҫ�ĳ�ʼ��
	AcquireSRWLockExclusive(&CInfoListSRWLock);
	if (ClientCount >= MAX_CLIENT)
	{
		ReleaseSRWLockExclusive(&CInfoListSRWLock);
		return 0;//����
	}
	pCLIENT_INFO CInfo = malloc(sizeof(CLIENT_INFO));
	CInfoList[ClientCount] = CInfo;
	CInfo->ListIndex = ClientCount++;

	ReleaseSRWLockExclusive(&CInfoListSRWLock);
	//������ˣ���ʼ��������Ϣ

	memset(CInfo, 0, sizeof(CLIENT_INFO));
	CInfo->PackHeader[0].len = sizeof(BYTE);
	CInfo->PackHeader[0].buf = &(CInfo->PackID);

	CInfo->PackHeader[1].len = sizeof(unsigned int);
	CInfo->PackHeader[1].buf = &(CInfo->PackLen);

	CInfo->PackParseState = PARSE_WAITFOR_HEADER;//�ȴ��������ݰ�ͷ
	//ʣ���Ǹ�....���յ���ͷ���ٴ���

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
	//�ӱ���ɾ��
	AcquireSRWLockExclusive(&CInfoListSRWLock);
	CInfoList[CInfo->ListIndex] = CInfoList[--ClientCount];
	CInfoList[CInfo->ListIndex]->ListIndex = CInfo->ListIndex;
	ReleaseSRWLockExclusive(&CInfoListSRWLock);

	free(CInfo);
	return TRUE;
}

