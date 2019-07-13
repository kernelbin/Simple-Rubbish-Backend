#include<Windows.h>
#include<process.h>
#include<WinSock2.h>
#include"Global.h"
#include"SocketEngine.h"
#include"ClientManager.h"
#include"PackStructDef.h"
#include"PackParser.h"
//����ά��socket���������У��̵߳��ȣ��Լ�������

BOOL InitSocket()
{
	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//����һ���µ��̣߳�������socket
	
	_beginthreadex(0, 0, ListenThread, 0, 0, 0);
	return TRUE;
}

BOOL FinSocket()
{
	//�ر�listen��socket�����ҵȴ��߳�ȫ���˳�

	pIOCPMODEPACK ModePack = CreateModePack(IO_EXIT);

	SYSTEM_INFO SysInfo;//����ȡ��CPU��������Ϣ
	GetSystemInfo(&SysInfo);

	for (int i = 0; i < SysInfo.dwNumberOfProcessors; i++)
	{
		PostQueuedCompletionStatus(hCompPort, 0, 0, (LPOVERLAPPED)ModePack);
		//TODO: �Ҿ��û�����Ҫ�����ֶ��ȴ�һ�¼����̳߳��׽���
	}
	
	//�������PostQueuedCompletionStatus������IOCP�̡߳�������Ҫһ���µ�Overlapped��ModePack
	//	Windows Via C++����sample

	closesocket(ListenSock);
	WaitForSingleObject(hListenThread, INFINITE);
	return 0;
}

void WINAPI ListenThread()
{
	//����IOCP�ˣ���Ҫ��ûʱ�䣩
	//���ң�����ʹ�ö�����socket������һ������һ���߳�ò�ƻ����Խ��ܡ�
	SOCKADDR_IN ServerAddr;

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ServerAddr.sin_port = htons(SOCK_PORT);

	ListenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	//����IO��ɶ˿�
	hCompPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	SYSTEM_INFO SysInfo;//����ȡ��CPU��������Ϣ
	GetSystemInfo(&SysInfo);

	for (int i = 0; i < SysInfo.dwNumberOfProcessors; i++)
	{
		HANDLE hCPThread = _beginthreadex(0, 0, CompletionPortMain, 0, 0, 0);
		CloseHandle(hCPThread);
	}

	int iret = bind(ListenSock, &ServerAddr, sizeof(SOCKADDR_IN));

	listen(ListenSock, SOCK_MAX_LISTEN);

	//��ʼѭ��accept
	while (1)
	{
		SOCKADDR_IN ClientAddr = { 0 };
		int AddrLen = sizeof(SOCKADDR_IN);
		
		SOCKET ClientSock = accept(ListenSock, (struct sockaddr*) & ClientAddr, &AddrLen);

		if ((signed int)(ClientSock) == INVALID_SOCKET)
		{
			//�����ˣ����ԭ��
			int err = WSAGetLastError();
			if (err == WSAEINTR)
			{
				//�������˳��߳���
				break;
			}
		}

		//�����ͻ��˶���
		pCLIENT_INFO CInfo = AllocClient();
		
		CInfo->ClientSock = ClientSock;
		CreateIoCompletionPort((HANDLE)ClientSock, hCompPort, CInfo, 0);

		
		//����OVERLAPPED�ṹ
		pIOCPMODEPACK ModePack = CreateModePack(IO_RECV);
		DWORD Flags = 0;
		
		int iret = WSARecv(ClientSock,
			CInfo->PackHeader,
			2,
			NULL,//MSDN��˵����ΪNULL�Ա������
			&Flags,
			(LPWSAOVERLAPPED)ModePack, 0);
		if (iret == 0)
		{
			//û����
		}
		else if (iret == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == ERROR_IO_PENDING)
			{
				//û����
			}
			else
			{
				//������
				SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("WSARecv����ִ�г���WSAGetLastError����ֵΪ"), err);
			}
		}
		else
		{
			//������
			SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("WSARecv����ִ�г���WSARecv����ֵΪ"), iret);
		}

	}
}


void __stdcall CompletionPortMain(void)
{
	DWORD ByteTrans;
	pCLIENT_INFO CInfo;
	IOCPMODEPACK * pModePack = 0;
	BOOL bExit = FALSE;
	while (1)
	{
		//TODO:ʹ��GetQueuedCompletionStatusEx���Խ�һ�������������
		BOOL bOK = GetQueuedCompletionStatus(hCompPort, &ByteTrans, (PULONG_PTR)& CInfo, (LPOVERLAPPED *)& pModePack, INFINITE);
		DWORD dwErr = GetLastError();
		if (bOK)
		{
			//����
			switch (pModePack->IOMode)
			{
			case IO_SEND:
				//���������ݳɹ�
				MessageBox(NULL, TEXT(""), TEXT(""), 0);
				ReleaseSRWLockExclusive(&(CInfo->WSASendLock));
				break;
			case IO_RECV:
				//�����������

				if (ByteTrans == 0)
				{
					//���ӶϿ�
					closesocket(CInfo->ClientSock);
					FreeClient(CInfo);
				}
				switch (CInfo->PackParseState)
				{
				case PARSE_WAITFOR_HEADER:
					//�����յ����ݰ�ͷ���������ڴ�׼�����ܰ�����
					if (CInfo->PackLen)
					{
						AdjustVBuf(CInfo->Data, CInfo->PackLen);
						//����WSABUF
						CInfo->PackBody.buf = CInfo->Data->Data;
						CInfo->PackBody.len = CInfo->PackLen;

						DWORD Flags = 0;
						pIOCPMODEPACK NewModePack = CreateModePack(IO_RECV);

						CInfo->PackParseState = PARSE_WAITFOR_PACKBODY;

						WSARecv(CInfo->ClientSock,
							&(CInfo->PackBody), 1,
							NULL,
							&Flags,
							(LPWSAOVERLAPPED)NewModePack, 0);

						PACK_LOGON TestPack;
						int PackConnReqType[1] = { VAR_STRING };
						TestPack.ClientSSID = malloc(1000);
						lstrcpyW(TestPack.ClientSSID, L"��qwq");
						//int PackConnReqType[1] = { VAR_STRING };
						printf("sending");
						ClientSendPackFunc(CInfo, 4, &TestPack, PackConnReqType, 1);
					}
					else
					{
						//TODO: �հ���������һ����ͷ
					}
					 
					break;

				case PARSE_WAITFOR_PACKBODY:
					//�����յ����ݰ����壬�������֮��ַ��¼�
					switch (CInfo->PackID)
					{
					case PACKID_LOGON:
					{
						PACK_LOGON LoginPack;
						int PackLoginType[1] = { VAR_STRING };
						ParsePack(CInfo->Data->Data, &LoginPack, PackLoginType);

						lstrcpyW(CInfo->ClientUUID, LoginPack.ClientSSID);

						FreePack(&LoginPack, PackLoginType);
						//MessageBoxW(0, TestPack.ClientSSID, TestPack.ClientSSID, 0);
						break;
					}
						
					}

					CInfo->PackParseState = PARSE_WAITFOR_HEADER;

					pIOCPMODEPACK NewModePack = CreateModePack(IO_RECV);

					DWORD Flags = 0;
					int iret = WSARecv(CInfo->ClientSock,
						CInfo->PackHeader, 2,
						NULL,
						&Flags,
						(LPWSAOVERLAPPED)NewModePack, 0);
					if (iret == 0)
					{
						//û����
					}
					else if (iret == SOCKET_ERROR)
					{
						int err = WSAGetLastError();
						if (err == ERROR_IO_PENDING)
						{
							//û����
						}
						else
						{
							//������
							SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("WSARecv����ִ�г���WSAGetLastError����ֵΪ"), err);
						}
					}
					else
					{
						//������
						SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("WSARecv����ִ�г���WSARecv����ֵΪ"), iret);
					}

					break;
				}
				break;

			case IO_EXIT:
				bExit = TRUE;
				break;
			}
		}
		else 
		{
			if (pModePack)
			{
				//�����ˣ�dwErr�Ǵ�����
				SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("GetQueuedCompletionStatus��������GetLastError����ֵΪ"), dwErr);
			}
			else
			{
				//��������INFINITE�������������һ�������ǳ�ʱ�������Ǵ���Ĳ�����Ч
				SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("GetQueuedCompletionStatus��������������Ч��GetLastError����ֵΪ"), dwErr);
			}
		}
		if (pModePack)
		{
			DeleteModePack(pModePack);
		}
		if (bExit)
		{
			return;//�˳��߳�
		}
		
	}
	return;
}



BOOL ClientSendPackFunc(pCLIENT_INFO CInfo,BYTE PackID, void** PackStruct, int TypeArray[],int Num)
{
	//ֻ��һ��send
	
	AcquireSRWLockExclusive(&(CInfo->WSASendLock));

	AdjustVBuf(CInfo->SendData, 0);
	printf("Num = %d", Num);
	WriteStructToVBufFunc(PackStruct, CInfo->SendData, TypeArray, Num);
	
	CInfo->SendPackLen = CInfo->SendData->Length;
	CInfo->SendPackID = PackID;
	CInfo->PackSend[0].buf = &(CInfo->SendPackID);
	CInfo->PackSend[0].len = sizeof(BYTE);
	CInfo->PackSend[1].buf = &(CInfo->SendPackLen);
	CInfo->PackSend[1].len = sizeof(unsigned int);
	CInfo->PackSend[2].buf = (CInfo->SendData->Data);
	CInfo->PackSend[2].len = CInfo->SendData->Length;

	printf("CInfo->SendData->Length = %d", CInfo->SendData->Length);
	DWORD Flags = 0;
	pIOCPMODEPACK ModePack = CreateModePack(IO_SEND);
	printf("sended");
	DWORD BytesSend = 0;
	int ret = WSASend(CInfo->ClientSock, CInfo->PackSend, 3, 0, 0, ModePack, 0);
	DWORD err = WSAGetLastError();
	printf("ret = %d\nerr = %d\n", ret,err);

	//ReleaseSRWLockExclusive(&(CInfo->WSASendLock));

}


pIOCPMODEPACK CreateModePack(int Mode)
{
	pIOCPMODEPACK ModePack = malloc(sizeof(IOCPMODEPACK));
	memset(ModePack, 0, sizeof(IOCPMODEPACK));
	ModePack->IOMode = Mode;
	return ModePack;
}


BOOL DeleteModePack(pIOCPMODEPACK ModePack)
{
	free(ModePack);
	return TRUE;
}


