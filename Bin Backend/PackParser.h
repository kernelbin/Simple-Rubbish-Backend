#pragma once
#include<Windows.h>
#include"PackStructDef.h"
#include"VBuffer.h"

#define ParsePack(Data,PackStruct,TypeArray) ParsePackFunc(Data, PackStruct, TypeArray, (sizeof(TypeArray)/sizeof(TypeArray[0])))
BOOL ParsePackFunc(BYTE* Data, void** PackStruct, int TypeArray[], int Num);

#define WriteStructToVBuf(PackStruct,Buffer,TypeArray) WriteStructToVBufFunc(PackStruct,Buffer,TypeArray,(sizeof(TypeArray)/sizeof(TypeArray[0])))
BOOL WriteStructToVBufFunc(void** PackStruct, pVBUF Buffer, int TypeArray[], int Num);

//BOOL WriteStructToVBufFunc(void * PackStruct, pVBUF Buffer, int TypeArray[], int Num);
