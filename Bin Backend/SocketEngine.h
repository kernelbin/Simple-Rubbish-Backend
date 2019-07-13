#pragma once
#include<Windows.h>
#include<WinSock2.h>
#include<stdio.h>
#include"ClientManager.h"

#define IO_SEND 1
#define IO_RECV 2
#define IO_EXIT 3
typedef struct __tagIocpModePack
{
	WSAOVERLAPPED Overlapped;
	int IOMode;
}IOCPMODEPACK, * pIOCPMODEPACK;



typedef struct __tagClientInfo CLIENT_INFO, * pCLIENT_INFO;

BOOL ClientSendPackFunc(pCLIENT_INFO CInfo, BYTE PackID, void** PackStruct, int TypeArray[], int Num);