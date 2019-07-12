#include<Windows.h>
#include<process.h>
#include<WinSock2.h>
#include"Global.h"
#include"SocketEngine.h"
#include"ClientManager.h"
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


	����������̳߳ز��ҵ���GetQueuedCompletionStatus

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
