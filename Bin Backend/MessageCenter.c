#include<Windows.h>
#include"Global.h"

#define HANDLE_COMMAND(Command) if(strcmp(Command,wParam) == 0)
BOOL InitMessageCenter()
{
	WNDCLASS wc = { 0 };
	wc.hInstance = GetModuleHandle(0);
	wc.lpfnWndProc = MessageCenterProc;
	wc.lpszClassName = TEXT("Message Center");
	RegisterClass(&wc);
	hMessageCenter = CreateWindow(TEXT("Message Center"), TEXT("Message Center"), 0, 0, 0, 0, 0, HWND_MESSAGE, 0, GetModuleHandle(0), 0);
	return TRUE;
}

LRESULT CALLBACK MessageCenterProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return 0;
	case WM_CONCOMMAND:
		//ָ����Ϣ
		HANDLE_COMMAND("help")
		{
			printf("helpmsg\n");
			break;
		}
		HANDLE_COMMAND("exit")
		{
			PostQuitMessage(0);
			break;
		}

		break;

	case WM_THROWEXCEPTION:
	{
		//��ʾ�쳣��Ϣ�������˳�
		TCHAR TextBuffer[1024];
		wsprintf(TextBuffer, TEXT("%s %d"), wParam, lParam);
		MessageBox(NULL, TextBuffer, TEXT("������Ϣ"), 0);
		PostQuitMessage(0);
		break;
	}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}