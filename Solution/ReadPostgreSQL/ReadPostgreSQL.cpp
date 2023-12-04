// ReadPostgreSQL.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
using namespace std;

#ifdef _DEBUG
	//
	// DEBUG
	//
#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define OS_FREE(pMem)			free(pMem)
#define OS_ALLOC(Size)			calloc(Size,1);
#define OS_DEBUG(...)			_cprintf(__VA_ARGS__)
#define DEBUG_BREAK()			__debugbreak()
#else
	//
	// RELEASE
	//
#define OS_FREE(pMem)			HeapFree(GetProcessHeap(), 0, pMem)
#define OS_ALLOC(Size)			HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size)
#define OS_DEBUG(...)			_cprintf(__VA_ARGS__)
#define DEBUG_BREAK()
#endif  // _DEBUG

#pragma pack(1)
typedef struct _PSTSQL_HEADER {
	WORD filler[6];
	WORD totalBase;
	WORD startBase;
} PSTSQLHEADER, * PPSTSQLHEADER;

typedef struct _PST_USER_PRIV {
	BYTE rolsuper;
	BYTE rolinherit;
	BYTE rolcreaterole;
	BYTE rolcreatedb;
	BYTE rolcanlogin;
	BYTE rolreplication;
	BYTE rolconnlimit;
	BYTE rolpassword;
}PSTUSERPRIV;

typedef struct _PSTSQL_USER {
	BYTE filler[28];
	UINT32 oid;
	BYTE name[64];
	PSTUSERPRIV priv;
	BYTE password_encryption[8];
	BYTE md5[32];
} PSTSQLUSER, * PPSTSQLUSER;


#pragma pack()
/**
 * @brief 解析 1260 文件内容
 * @detail
 *
 * @param mapAddress 1260 文件内容
 * @param StopSize 停止读取的位置
 * @return void
 */
void ListUsers(PVOID mapAddress, int StopSize)
{
	BYTE UserFlag[8] = { 0xFF,0xFF,0xFF,0xFF,0x49,0x6D,0x64,0x35 };
	PPSTSQLHEADER pstHeader = NULL;
	PPSTSQLUSER nextRecordPtr = NULL;
	PVOID buffer = NULL;
	size_t num_read = 0;
	int FlagSize = 0x14;
	PPSTSQLUSER pUser;

	pstHeader = (PPSTSQLHEADER)mapAddress;

	if (pstHeader->startBase == NULL) goto End;

	// 处理数据
	buffer = (PVOID)((PBYTE)mapAddress+(DWORD)pstHeader->startBase);
	FlagSize += (int)pstHeader->startBase;
	pUser = (PPSTSQLUSER)buffer;
	while (FlagSize < StopSize)
	{
		int size = sizeof(PSTSQLUSER);

		if (memcmp(pUser->password_encryption, UserFlag, 8))
		{
			printf("[!]Maybe it's not User");
			break;
		}

		num_read++;
		FlagSize += size;
		printf("%d %s %s\n", pUser->oid, (char*)pUser->name, (char*)pUser->md5);

		buffer = (PVOID)((PBYTE)mapAddress + ((DWORD)pstHeader->startBase + size * num_read));
		pUser = (PPSTSQLUSER)buffer;
		continue;
	}

End:
	return;
}

int main(int argc, const char* argv[])
{
	cout << sizeof(PSTSQLUSER) << endl;
	cout << sizeof(CHAR) << endl;

	if (argc != 2)
	{
		printf("\nLoad the PostgreSQL and print the data.\n");
		printf("Author:holdyounger\n");
		printf("Usage:\n");
		printf("     %s <file path of data file>\n", argv[0]);
		printf("eg:\n");
		printf("     %s C:\\Program Files (x86)\\PostgreSQL\\10\\data\\global\\1260\n\n", argv[0]);
		printf("[!]Wrong parameter\n");
		return 0;
	}

	FILE* fp;
	int err = fopen_s(&fp, argv[1], "a+");
	if (err != 0)
	{
		printf("openfile error!");
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	unsigned char* buf = new unsigned char[len];
	fseek(fp, 0, SEEK_SET);
	fread(buf, len, 1, fp);
	ListUsers(buf, len);
	fclose(fp);

	delete buf;
	getchar();
	return 0;
}