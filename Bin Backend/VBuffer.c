#include<Windows.h>
#include<malloc.h>
#include"VBuffer.h"


pVBUF AllocVBuf()
{
	pVBUF pVBuf= malloc(sizeof(VBUF));
	memset(pVBuf, 0, sizeof(VBUF));

	return pVBuf;
}

BOOL FreeVBuf(pVBUF pVBuf)
{
	if (pVBuf->Data)
	{
		free(pVBuf->Data);
	}
	free(pVBuf);
	return TRUE;
}

BOOL AdjustVBuf(pVBUF pVBuf, DWORD Size)
{
	pVBuf->Length = Size;
	pVBuf->Capibility = VBufGetCorrectSize(Size);
	
	if (pVBuf->Capibility)
	{
		if (!pVBuf->Data)
		{
			pVBuf->Data = malloc(pVBuf->Capibility);
		}
		else
		{
			pVBuf->Data = realloc(pVBuf->Data, pVBuf->Capibility);
		}
	}
	else
	{
		free(pVBuf->Data);//free֧�ֲ���NULL
		pVBuf->Data = NULL;
	}
	

	return 0;
}

BOOL AddSizeVBuf(pVBUF pVBuf,DWORD AddSize)
{
	return AdjustVBuf(pVBuf, pVBuf->Length + AddSize);
}

DWORD VBufGetCorrectSize(DWORD Size)
{
	int iBit = 0;
	for (DWORD iBit = 1 << (sizeof(DWORD) * 8 - 1); iBit; iBit >>= 1)
	{
		if (iBit & Size)
		{
			return iBit << 1;
		}
	}
	return 0;
}