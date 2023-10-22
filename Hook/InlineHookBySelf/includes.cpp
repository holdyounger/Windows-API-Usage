#include "includes.h"

BYTE* Hook(BYTE* pbyFun, BYTE* pbyHkFun, SIZE_T szHkSize)
{
	if (szHkSize < 5)
		return nullptr;

	// 申请内存 保存字节 跳转回原函数 目标地址-自身地址-5
	BYTE* pbtRetAddr = (BYTE*)VirtualAlloc(NULL, szHkSize + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!pbtRetAddr)
		return nullptr;

	memcpy(pbtRetAddr, pbyFun, szHkSize);

	*(BYTE*)((DWORD)pbtRetAddr + szHkSize) = 0xE9; // JMP
	*(BYTE*)((DWORD)pbtRetAddr + szHkSize + 1) = pbyFun - pbtRetAddr - 5;

	// 修改目标函数的内存保护 跳转到我们的函数内
	DWORD dwOldProtect;
	VirtualProtect(pbyFun, szHkSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*(BYTE*)pbyFun = 0xE9;
	*(BYTE*)((DWORD)pbyFun + 1) = pbyHkFun - pbyFun - 5;
	VirtualProtect(pbyFun, szHkSize, dwOldProtect, &dwOldProtect);

	return pbtRetAddr;
}

BYTE* unHook(BYTE* pbyFun, BYTE* pbtOldBuffer, SIZE_T szHkSize)
{
	DWORD dwOldProtect;
	VirtualProtect(pbyFun, szHkSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy(pbyFun, pbtOldBuffer, szHkSize);
	VirtualProtect(pbyFun, szHkSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	return nullptr;
}
