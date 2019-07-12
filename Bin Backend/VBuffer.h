#include<Windows.h>

typedef struct __tagVBuffer
{
	DWORD Length;
	DWORD Capibility;
	PBYTE Data;
}VBUFFER,VBUF,*pVBUF;

pVBUF AllocVBuf();
BOOL FreeVBuf(pVBUF pVBuf);
BOOL AdjustVBuf(pVBUF pVBuf, DWORD Size);
DWORD VBufGetCorrectSize(DWORD Size);
