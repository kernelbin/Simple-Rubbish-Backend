#include<Windows.h>
#include<malloc.h>
#include"VBuffer.h"
#include"PackStructDef.h"

BOOL ParsePackFunc(BYTE* Data, void ** PackStruct,int TypeArray[],int Num)
{
	//TODO: 根据传入的类型数组，把Data里的东西解析进Pack

	PBYTE PackStructPointer = PackStruct;
	for (int iPack = 0; iPack < Num; iPack++)
	{
		switch (TypeArray[iPack])
		{
		case VAR_INT:
		{
			int* pInt = Data;
			*((PINT)PackStructPointer) = *pInt;
			PackStructPointer += sizeof(int);
			Data += sizeof(int);
		}
			break;
		case VAR_STRING:
		{
			unsigned short * pStrLen;
			pStrLen = (unsigned short*)Data;
			Data += sizeof(unsigned short);
			PackStructPointer += sizeof(unsigned short);
			PBYTE UTF8Str = Data;

			int UTF16Len = MultiByteToWideChar(CP_UTF8, 0, UTF8Str, (*pStrLen), 0, 0);
			*((PWCHAR *)PackStructPointer) = malloc(UTF16Len * sizeof(WCHAR));
			MultiByteToWideChar(CP_UTF8, 0, UTF8Str, (*pStrLen), *((PWCHAR*)PackStructPointer), UTF16Len);
			PackStructPointer += sizeof(PWCHAR);
			Data += *pStrLen;
		}
			break;
		}
	}
}

BOOL FreePackFunc(void* PackStruct, int TypeArray[], int Num)
{
	//释放这个包
	//TODO: 根据传入的类型数组，把Data里的东西解析进Pack

	PBYTE PackStructPointer = PackStruct;
	for (int iPack = 0; iPack < Num; iPack++)
	{
		switch (TypeArray[iPack])
		{
		case VAR_INT:
		{
			PackStructPointer += sizeof(int);
		}
		break;
		case VAR_STRING:
		{
			PackStructPointer += sizeof(unsigned short);
			free(*((PWCHAR*)PackStructPointer));
			PackStructPointer += sizeof(PWCHAR);
		}
		break;
		}
	}
}


BOOL WriteStructToVBufFunc(void** PackStruct, pVBUF Buffer, int TypeArray[], int Num)
{
	PBYTE PackStructPointer = PackStruct;
	int iDataMove = 0;
	for (int iPack = 0; iPack < Num; iPack++)
	{
		switch (TypeArray[iPack])
		{
		case VAR_INT:
		{
			//int* pInt = Data;
			AddSizeVBuf(Buffer, sizeof(int));
			*((int*)(Buffer->Data + iDataMove)) = *((PINT)PackStructPointer);// = *pInt;
			PackStructPointer += sizeof(int);
			iDataMove += sizeof(int);
		}
		break;
		case VAR_STRING:
		{
			AddSizeVBuf(Buffer, sizeof(unsigned short));
			int cbStrLen = WideCharToMultiByte(CP_UTF8, 0, *((PWCHAR*)PackStructPointer), lstrlenW(*((PWCHAR*)PackStructPointer)), 0, 0, 0, 0);
			*((unsigned short*)(Buffer->Data + iDataMove)) = cbStrLen;
			//pStrLen = (unsigned short*)Data;
			iDataMove += sizeof(unsigned short);
			//PackStructPointer += sizeof(unsigned short);
			
			//这里，转换字符串并写入
			AddSizeVBuf(Buffer, cbStrLen);

			WideCharToMultiByte(CP_UTF8, 0,
				*((PWCHAR*)PackStructPointer), lstrlenW(*((PWCHAR*)PackStructPointer)),
				(Buffer->Data + iDataMove), cbStrLen, 0, 0);
			iDataMove += cbStrLen;
			PackStructPointer += sizeof(PWCHAR);
		}
		break;
		}
	}
}