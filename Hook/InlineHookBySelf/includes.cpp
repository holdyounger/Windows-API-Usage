#include "includes.h"

BYTE* Hook(BYTE* pbyFun, BYTE* pbyHkFun, SIZE_T szHkSize)
{
	if (szHkSize < 5)
		return nullptr;

	// �����ڴ� �����ֽ� ��ת��ԭ���� Ŀ���ַ-�����ַ-5
	BYTE* pbtRetAddr = (BYTE*)VirtualAlloc(NULL, szHkSize + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!pbtRetAddr)
		return nullptr;

	memcpy(pbtRetAddr, pbyFun, szHkSize);

	*(BYTE*)((DWORD)pbtRetAddr + szHkSize) = 0xE9; // JMP
	*(BYTE*)((DWORD)pbtRetAddr + szHkSize + 1) = pbyFun - pbtRetAddr - 5;

	// �޸�Ŀ�꺯�����ڴ汣�� ��ת�����ǵĺ�����
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
