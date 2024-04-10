// FileMappingTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
using namespace std;

int main()
{
	HANDLE hMap;
	PINT lpMapAddr;

	// get file mapping's handle 
	hMap = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,	// 权限：所有
		FALSE,					// 不继承句柄
		L"LYSM_Map"				// 名字
	);
	if (NULL == hMap) {
		// init
		hMap = CreateFileMapping(
			INVALID_HANDLE_VALUE,	// 不是真实的文件，所以写 INVALID_HANDLE_VALUE
			NULL,					// 同上
			PAGE_READWRITE,			// 该句柄对文件映射可读可写
			0,						// 与下一个参数连用，表示文件映射的大小
			1024,					// 与上一个参数连用，表示文件映射的大小
			L"LYSM_Map"				// 名字
		);
		if (NULL == hMap) {
			goto end;
		}
		lpMapAddr = (PINT)MapViewOfFile(
			hMap,						// 文件映射句柄
			FILE_MAP_ALL_ACCESS,		// 权限：所有
			0,							// 与下一个参数连用，表示文件映射起始地址偏移
			0,							// 与上一个参数连用，表示文件映射起始地址偏移
			0							// 映射整个文件映射对象
		);
		*lpMapAddr = 1;
	}

	// get file mapping's address
	lpMapAddr = (PINT)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	// read 
	cout << "instance's count : " << *lpMapAddr << endl;

	// write
	*lpMapAddr = *lpMapAddr + 1;


	cout << "finished." << endl;
	getchar();
end:
	if (hMap)
		CloseHandle(hMap);
	getchar();
	return 0;
}