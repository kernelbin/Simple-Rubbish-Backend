#include<Windows.h>
#include<process.h>
#include<WinSock2.h>
#include"Global.h"
#include"SocketEngine.h"
#include"ClientManager.h"
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


	这里！！创建线程池并且调用GetQueuedCompletionStatus

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
