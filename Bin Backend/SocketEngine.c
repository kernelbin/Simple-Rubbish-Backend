#include<Windows.h>
#include<process.h>
#include<WinSock2.h>
#include"Global.h"
#include"SocketEngine.h"
#include"ClientManager.h"
#include"PackStructDef.h"
#include"PackParser.h"
//负责维护socket的正常运行，线程调度，以及解包封包

BOOL InitSocket()
{
	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//创建一个新的线程，来监听socket
	
	_beginthreadex(0, 0, ListenThread, 0, 0, 0);
	return TRUE;
}

BOOL FinSocket()
{
	//关闭listen的socket，并且等待线程全部退出

	pIOCPMODEPACK ModePack = CreateModePack(IO_EXIT);

	SYSTEM_INFO SysInfo;//用来取得CPU数量等信息
	GetSystemInfo(&SysInfo);

	for (int i = 0; i < SysInfo.dwNumberOfProcessors; i++)
	{
		PostQueuedCompletionStatus(hCompPort, 0, 0, (LPOVERLAPPED)ModePack);
		//TODO: 我觉得还是需要我们手动等待一下几个线程彻底结束
	}
	
	//这里！！！PostQueuedCompletionStatus来结束IOCP线程。可能需要一个新的Overlapped的ModePack
	//	Windows Via C++里有sample

	closesocket(ListenSock);
	WaitForSingleObject(hListenThread, INFINITE);
	return 0;
}

void WINAPI ListenThread()
{
	//不用IOCP了（主要是没时间）
	//而且，考虑使用短链接socket，所以一个请求一个线程貌似还可以接受。
	SOCKADDR_IN ServerAddr;

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ServerAddr.sin_port = htons(SOCK_PORT);

	ListenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	//创建IO完成端口
	hCompPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	SYSTEM_INFO SysInfo;//用来取得CPU数量等信息
	GetSystemInfo(&SysInfo);

	for (int i = 0; i < SysInfo.dwNumberOfProcessors; i++)
	{
		HANDLE hCPThread = _beginthreadex(0, 0, CompletionPortMain, 0, 0, 0);
		CloseHandle(hCPThread);
	}

	int iret = bind(ListenSock, &ServerAddr, sizeof(SOCKADDR_IN));

	listen(ListenSock, SOCK_MAX_LISTEN);

	//开始循环accept
	while (1)
	{
		SOCKADDR_IN ClientAddr = { 0 };
		int AddrLen = sizeof(SOCKADDR_IN);
		
		SOCKET ClientSock = accept(ListenSock, (struct sockaddr*) & ClientAddr, &AddrLen);

		if ((signed int)(ClientSock) == INVALID_SOCKET)
		{
			//出错了，检察原因
			int err = WSAGetLastError();
			if (err == WSAEINTR)
			{
				//看来该退出线程了
				break;
			}
		}

		//创建客户端对象
		pCLIENT_INFO CInfo = AllocClient();
		
		CInfo->ClientSock = ClientSock;
		CreateIoCompletionPort((HANDLE)ClientSock, hCompPort, CInfo, 0);

		
		//创建OVERLAPPED结构
		pIOCPMODEPACK ModePack = CreateModePack(IO_RECV);
		DWORD Flags = 0;
		
		int iret = WSARecv(ClientSock,
			CInfo->PackHeader,
			2,
			NULL,//MSDN上说设置为NULL以避免错误
			&Flags,
			(LPWSAOVERLAPPED)ModePack, 0);
		if (iret == 0)
		{
			//没问题
		}
		else if (iret == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == ERROR_IO_PENDING)
			{
				//没问题
			}
			else
			{
				//出错了
				SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("WSARecv函数执行出错，WSAGetLastError返回值为"), err);
			}
		}
		else
		{
			//出错了
			SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("WSARecv函数执行出错，WSARecv返回值为"), iret);
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
		//TODO:使用GetQueuedCompletionStatusEx可以进一步大幅提升性能
		BOOL bOK = GetQueuedCompletionStatus(hCompPort, &ByteTrans, (PULONG_PTR)& CInfo, (LPOVERLAPPED *)& pModePack, INFINITE);
		DWORD dwErr = GetLastError();
		if (bOK)
		{
			//正常
			switch (pModePack->IOMode)
			{
			case IO_SEND:
				//处理发送数据成功
				MessageBox(NULL, TEXT(""), TEXT(""), 0);
				ReleaseSRWLockExclusive(&(CInfo->WSASendLock));
				break;
			case IO_RECV:
				//处理接受数据

				if (ByteTrans == 0)
				{
					//连接断开
					closesocket(CInfo->ClientSock);
					FreeClient(CInfo);
				}
				switch (CInfo->PackParseState)
				{
				case PARSE_WAITFOR_HEADER:
					//处理收到数据包头部。调整内存准备接受包主体
					if (CInfo->PackLen)
					{
						AdjustVBuf(CInfo->Data, CInfo->PackLen);
						//设置WSABUF
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
						lstrcpyW(TestPack.ClientSSID, L"喵qwq");
						//int PackConnReqType[1] = { VAR_STRING };
						printf("sending");
						ClientSendPackFunc(CInfo, 4, &TestPack, PackConnReqType, 1);
					}
					else
					{
						//TODO: 空包。接收下一个包头
					}
					 
					break;

				case PARSE_WAITFOR_PACKBODY:
					//处理收到数据包主体，解析完毕之后分发事件
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
						//没问题
					}
					else if (iret == SOCKET_ERROR)
					{
						int err = WSAGetLastError();
						if (err == ERROR_IO_PENDING)
						{
							//没问题
						}
						else
						{
							//出错了
							SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("WSARecv函数执行出错，WSAGetLastError返回值为"), err);
						}
					}
					else
					{
						//出错了
						SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("WSARecv函数执行出错，WSARecv返回值为"), iret);
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
				//出错了，dwErr是错误码
				SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("GetQueuedCompletionStatus函数出错。GetLastError返回值为"), dwErr);
			}
			else
			{
				//我设置了INFINITE，所以如果出错一定不会是超时。看来是传入的参数无效
				SendMessage(hMessageCenter, WM_THROWEXCEPTION, TEXT("GetQueuedCompletionStatus函数出错，参数无效。GetLastError返回值为"), dwErr);
			}
		}
		if (pModePack)
		{
			DeleteModePack(pModePack);
		}
		if (bExit)
		{
			return;//退出线程
		}
		
	}
	return;
}



BOOL ClientSendPackFunc(pCLIENT_INFO CInfo,BYTE PackID, void** PackStruct, int TypeArray[],int Num)
{
	//只能一个send
	
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


