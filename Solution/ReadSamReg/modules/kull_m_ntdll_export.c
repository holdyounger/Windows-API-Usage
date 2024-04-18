#include "kull_m_ntdll_export.h"

FuncPtr_RtlInitUnicodeString		Ptr_RtlInitUnicodeString = NULL;
FuncPtr_RtlEqualString				Ptr_RtlEqualString = NULL;
FuncPtr_RtlEqualUnicodeString		Ptr_RtlEqualUnicodeString = NULL;
FuncPtr_RtlFreeUnicodeString		Ptr_RtlFreeUnicodeString = NULL;
FuncPtr_RtlStringFromGUID			Ptr_RtlStringFromGUID = NULL;
FuncPtr_RtlAppendUnicodeToString	Ptr_RtlAppendUnicodeToString = NULL;
FuncPtr_RtlGetNtVersionNumbers		Ptr_RtlGetNtVersionNumbers = NULL;
FuncPtr_RtlGetCurrentPeb			Ptr_RtlGetCurrentPeb = NULL;
FuncPtr_RtlAdjustPrivilege			Ptr_RtlAdjustPrivilege = NULL;
FuncPtr_NtQuerySystemInformation	Ptr_NtQuerySystemInformation = NULL;
FuncPtr_NtQueryInformationProcess	Ptr_NtQueryInformationProcess = NULL;
FuncPtr_MD4Init						Ptr_MD4Init = NULL;
FuncPtr_MD4Update					Ptr_MD4Update = NULL;
FuncPtr_MD4Final					Ptr_MD4Final = NULL;

FuncPtr_MD5INIT						Ptr_MD5Init = NULL;
FuncPtr_MD5UPDATE					Ptr_MD5Update = NULL;
FuncPtr_MD5FINAL					Ptr_MD5Final = NULL;

HMODULE RmLoadLibrary(LPCWSTR lpLibFileName)
{
	// Remove current directory from DLL search path
	SetDllDirectory(L"");

	SetLastError(ERROR_SUCCESS);
	HMODULE hLib = LoadLibrary(lpLibFileName);

	return hLib;
}

BOOL kull_m_ntdll_init()
{
	// Create MD5 digest from command line
	HMODULE cryptDll = RmLoadLibrary(L"cryptdll.dll");
	if (cryptDll)
	{
		FuncPtr_MD5INIT		Ptr_MD5Init = (FuncPtr_MD5INIT)GetProcAddress(cryptDll, "MD5Init");
		FuncPtr_MD5UPDATE	Ptr_MD5Update = (FuncPtr_MD5UPDATE)GetProcAddress(cryptDll, "MD5Update");
		FuncPtr_MD5FINAL	Ptr_MD5Final = (FuncPtr_MD5FINAL)GetProcAddress(cryptDll, "MD5Final");
	}
	//hNtdll不需要释放
	HANDLE hNtdll = GetModuleHandle(L"ntdll.dll");
	if (hNtdll)
	{
		BOOL bSuc = TRUE;
		Ptr_RtlInitUnicodeString = (FuncPtr_RtlInitUnicodeString)GetProcAddress(hNtdll, "RtlInitUnicodeString");
		if (!Ptr_RtlInitUnicodeString)
		{
			kprintf(L"Ptr_RtlInitUnicodeString is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_RtlEqualString = (FuncPtr_RtlEqualString)GetProcAddress(hNtdll, "RtlEqualString");
		if (!Ptr_RtlEqualString)
		{
			kprintf(L"Ptr_RtlEqualString is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_RtlEqualUnicodeString = (FuncPtr_RtlEqualUnicodeString)GetProcAddress(hNtdll, "RtlEqualUnicodeString");
		if (!Ptr_RtlEqualUnicodeString)
		{
			kprintf(L"Ptr_RtlEqualUnicodeString is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_RtlFreeUnicodeString = (FuncPtr_RtlFreeUnicodeString)GetProcAddress(hNtdll, "RtlFreeUnicodeString");
		if (!Ptr_RtlFreeUnicodeString)
		{
			kprintf(L"Ptr_RtlFreeUnicodeString is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_RtlStringFromGUID = (FuncPtr_RtlStringFromGUID)GetProcAddress(hNtdll, "RtlStringFromGUID");
		if (!Ptr_RtlStringFromGUID)
		{
			kprintf(L"Ptr_RtlStringFromGUID is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_RtlAppendUnicodeToString = (FuncPtr_RtlAppendUnicodeToString)GetProcAddress(hNtdll, "RtlAppendUnicodeToString");
		if (!Ptr_RtlStringFromGUID)
		{
			kprintf(L"Ptr_RtlAppendUnicodeToString is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_RtlGetNtVersionNumbers = (FuncPtr_RtlGetNtVersionNumbers)GetProcAddress(hNtdll, "RtlGetNtVersionNumbers");
		if (!Ptr_RtlGetNtVersionNumbers)
		{
			kprintf(L"Ptr_RtlGetNtVersionNumbers is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_RtlGetCurrentPeb = (FuncPtr_RtlGetCurrentPeb)GetProcAddress(hNtdll, "RtlGetCurrentPeb");
		if (!Ptr_RtlGetCurrentPeb)
		{
			kprintf(L"Ptr_RtlGetCurrentPeb is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_RtlAdjustPrivilege = (FuncPtr_RtlAdjustPrivilege)GetProcAddress(hNtdll, "RtlAdjustPrivilege");
		if (!Ptr_RtlAdjustPrivilege)
		{
			kprintf(L"Ptr_RtlAdjustPrivilege is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_NtQuerySystemInformation = (FuncPtr_NtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation");
		if (!Ptr_NtQuerySystemInformation)
		{
			kprintf(L"Ptr_NtQuerySystemInformation is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_NtQueryInformationProcess = (FuncPtr_NtQueryInformationProcess)GetProcAddress(hNtdll, "NtQueryInformationProcess");
		if (!Ptr_NtQueryInformationProcess)
		{
			kprintf(L"Ptr_NtQueryInformationProcess is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_MD4Init = (FuncPtr_MD4Init)GetProcAddress(hNtdll, "MD4Init");
		if (!Ptr_MD4Init)
		{
			kprintf(L"Ptr_MD4Init is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_MD4Update = (FuncPtr_MD4Update)GetProcAddress(hNtdll, "MD4Update");
		if (!Ptr_MD4Update)
		{
			kprintf(L"Ptr_MD4Update is NULL.\n");
			bSuc = FALSE;
		}
		Ptr_MD4Final = (FuncPtr_MD4Final)GetProcAddress(hNtdll, "MD4Final");
		if (!Ptr_MD4Final)
		{
			kprintf(L"Ptr_MD4Final is NULL.\n");
			bSuc = FALSE;
		}
		return bSuc;
	}

	return FALSE;
}