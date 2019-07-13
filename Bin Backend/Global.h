#pragma once

#include<Windows.h>
#include<stdio.h>
#include<WinSock2.h>
#include<malloc.h>
#include"ClientManager.h"
#include"SocketEngine.h"
#include"PackParser.h"

#pragma comment(lib,"Ws2_32.lib")


//���Ʊ���Win32�����ǿ���̨����ģ�ע�͵���һ�оͱ�Ϊ�������̨
#define APP_TYPE_WINDOWS



#ifndef APP_TYPE_WINDOWS
#define APP_TYPE_CONSOLE
#endif
#ifdef APP_TYPE_WINDOWS
#pragma comment( linker, "/subsystem:windows")
#endif
#ifdef APP_TYPE_CONSOLE
#pragma comment( linker, "/subsystem:console")
#endif

//ָ������̨�Ƿ�򿪵ı���
BOOL bConsoleOpen;

//����̨�ļ������

static FILE* std_out = 0;
static FILE* std_in = 0;


//����̨�߳�

void WINAPI ConsoleThread();

HANDLE hConThread;

//����̨������
BOOL InitConsole();
BOOL OpenConsole();
BOOL CloseConsole();
BOOL TerminateConThread();



//��Ϣ��������
LRESULT CALLBACK MessageCenterProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND hMessageCenter;

BOOL InitMessageCenter();

#define WM_CONCOMMAND (WM_USER + 1)
#define WM_THROWEXCEPTION (WM_USER + 2) //wParam�Ǵ�������������lParam�Ǵ�����





//socket���
void WINAPI ListenThread();

HANDLE hListenThread;

SOCKET ListenSock;

HANDLE hCompPort;//��ɶ˿ھ��

#define SOCK_PORT 23456
#define SOCK_MAX_LISTEN 255
BOOL InitSocket();


pIOCPMODEPACK CreateModePack(int Mode);

BOOL DeleteModePack(pIOCPMODEPACK ModePack);




void __stdcall CompletionPortMain(void);//��ɶ˿�