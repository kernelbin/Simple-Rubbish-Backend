#include<Windows.h>
#include<stdio.h>
#include<process.h>
#include"Global.h"

void WINAPI ConsoleThread();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{

	PACK_CONNREQ strct;
	PACK_TEST t;
	t.test = 123;
	strct.BinDescription = malloc(2345);
	lstrcpyW(strct.BinDescription, L"test");
	pVBUF vbuf = AllocVBuf();
	int tarr[1] = { VAR_INT };
	//debug”√
	WriteStructToVBufFunc(&t, vbuf ,tarr ,1 );


	return 0;


















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
