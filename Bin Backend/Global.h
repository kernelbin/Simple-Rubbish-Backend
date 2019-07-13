#pragma once

#include<Windows.h>
#include<stdio.h>
#include<WinSock2.h>
#include<malloc.h>
#include"ClientManager.h"
#include"SocketEngine.h"
#include"PackParser.h"

#pragma comment(lib,"Ws2_32.lib")


//控制编译Win32程序还是控制台程序的，注释掉这一行就变为编译控制台
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

//指明控制台是否打开的变量
BOOL bConsoleOpen;

//控制台文件流句柄

static FILE* std_out = 0;
static FILE* std_in = 0;


//控制台线程

void WINAPI ConsoleThread();

HANDLE hConThread;

//控制台管理函数
BOOL InitConsole();
BOOL OpenConsole();
BOOL CloseConsole();
BOOL TerminateConThread();



//消息处理中心
LRESULT CALLBACK MessageCenterProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND hMessageCenter;

BOOL InitMessageCenter();

#define WM_CONCOMMAND (WM_USER + 1)
#define WM_THROWEXCEPTION (WM_USER + 2) //wParam是错误文字描述，lParam是错误码





//socket相关
void WINAPI ListenThread();

HANDLE hListenThread;

SOCKET ListenSock;

HANDLE hCompPort;//完成端口句柄

#define SOCK_PORT 23456
#define SOCK_MAX_LISTEN 255
BOOL InitSocket();


pIOCPMODEPACK CreateModePack(int Mode);

BOOL DeleteModePack(pIOCPMODEPACK ModePack);




void __stdcall CompletionPortMain(void);//完成端口