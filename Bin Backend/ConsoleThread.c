#include<Windows.h>
#include<process.h>
#include"Global.h"



BOOL InitConsole()
{
#ifdef APP_TYPE_CONSOLE
	hConThread = _beginthreadex(0, 0, ConsoleThread, 0, 0, 0);
	bConsoleOpen = TRUE;
#else
	bConsoleOpen = FALSE;
	std_out = 0;
	std_in = 0;

#endif
	return TRUE;
}

BOOL OpenConsole()
{
#ifdef APP_TYPE_CONSOLE
	return FALSE;
#else

	if (bConsoleOpen)
	{
		return TRUE;
	}
	AllocConsole();
	
	freopen_s(&std_out, "CONOUT$", "w", stdout);
	freopen_s(&std_in, "CONIN$", "r", stdin);
	hConThread = _beginthreadex(0, 0, ConsoleThread, 0, 0, 0);
	bConsoleOpen = TRUE;

	return TRUE;
#endif
}

BOOL CloseConsole()
{
#ifdef APP_TYPE_CONSOLE
	return FALSE;
#else
	if (!bConsoleOpen)
	{
		return TRUE;
	}
	FreeConsole();
	
	fclose(std_out);
	std_out = 0;
	fclose(std_in);
	std_in = 0;

	WaitForSingleObject(hConThread, INFINITE);
	//TerminateThread(hConThread, 0);//强制结束
	
	hConThread = 0;

	
	bConsoleOpen = FALSE;
	return TRUE;
#endif
}

void WINAPI ConsoleThread()
{
	//编写这个线程的代码的时候，要非常小心。
	//这个线程（Win32模式下）随时可能毫无征兆的死掉

	printf("\
**********************************************************************\n\
* *Bin Backend v 0.0.1\n\
* *Copyright (c) 2019 yh\n\
* *********************************************************************\n");
	while (1)
	{
		char CommandBuffer[1024];
		int err = scanf_s("%s", CommandBuffer, sizeof(CommandBuffer));
		if (err == EOF)
		{
			break;
		}
		SendMessage(hMessageCenter, WM_CONCOMMAND, CommandBuffer, strlen(CommandBuffer));
		puts("指令已经发送");
	}
	return;
}

BOOL TerminateConThread()
{
	FreeConsole();

	WaitForSingleObject(hConThread, INFINITE);
	return 0;
}