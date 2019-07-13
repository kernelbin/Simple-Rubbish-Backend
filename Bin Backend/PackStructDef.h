#pragma once
#include<Windows.h>

//定义字段

#define VAR_INT 1
#define VAR_UINT 2
#define VAR_SHORT 3
#define VAR_USHORT 4
#define VAR_BOOL 5
#define VAR_BYTE 6
#define VAR_UBYTE 7
#define VAR_FLOAT 8

#define VAR_STRING 9




#define PACKID_CONNREQ 1
//定义包结构体
typedef struct __tagPackConnReq
{
	PWCHAR BinDescription;
}PACK_CONNREQ;


typedef struct
{
	int test;
}PACK_TEST;
