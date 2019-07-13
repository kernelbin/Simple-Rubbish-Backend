#include<Windows.h>
#include<stdio.h>
#include<process.h>
#include"Global.h"

#pragma comment(lib,"boost.lib")

void WINAPI ConsoleThread();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	InitMessageCenter();


	InitConsole();

//¡Ÿ ±¥˙¬Î
#ifdef APP_TYPE_WINDOWS
	OpenConsole();
#endif
	

	InitClientManager();


	InitSocket();
	



	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}






	FinSocket();

#ifdef APP_TYPE_WINDOWS
	CloseConsole();
#else
	TerminateConThread();
#endif
	return 0;
}
int main()
{
	return WinMain(0, 0, 0, 0);
}
