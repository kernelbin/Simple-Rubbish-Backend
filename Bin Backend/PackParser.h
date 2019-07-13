#pragma once
#include<Windows.h>
#include"PackStructDef.h"

#define ParsePack(Data,PackStruct,TypeArray) ParsePackFunc(Data, PackStruct, TypeArray, (sizeof(TypeArray)/sizeof(TypeArray[0])))
BOOL ParsePackFunc(BYTE* Data, BYTE* PackStruct, int TypeArray[], int Num);
//BOOL WriteStructToVBufFunc(void * PackStruct, pVBUF Buffer, int TypeArray[], int Num);
