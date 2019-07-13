#include<Windows.h>
#include<malloc.h>
#include"PackStructDef.h"

BOOL ParsePackFunc(BYTE* Data, BYTE * PackStruct,int TypeArray[],int Num)
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
			*((PINT*)PackStructPointer) = *pInt;
			Data += sizeof(int);
		}
			break;
		case VAR_STRING:
		{
			unsigned short * pStrLen;
			pStrLen = (unsigned short*)Data;
			Data += sizeof(unsigned short);
			PBYTE UTF8Str = Data;

			int UTF16Len = MultiByteToWideChar(CP_UTF8, 0, UTF8Str, (*pStrLen), 0, 0);
			*((PWCHAR *)PackStructPointer) = malloc(UTF16Len * sizeof(WCHAR));
			MultiByteToWideChar(CP_UTF8, 0, UTF8Str, (*pStrLen), *((PWCHAR*)PackStructPointer), UTF16Len);

			Data += sizeof(PWCHAR);
		}
			break;
		}
	}
}

BOOL FreePack(void* PackStruct, int TypeArray[], int Num)
{
	//释放这个包

}