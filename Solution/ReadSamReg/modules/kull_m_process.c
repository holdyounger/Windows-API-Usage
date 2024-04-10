/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kull_m_process.h"
#include "kull_m_ntdll_export.h"

NTSTATUS kull_m_process_NtQuerySystemInformation(SYSTEM_INFORMATION_CLASS informationClass, PVOID buffer, ULONG informationLength)
{
	NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;
	DWORD sizeOfBuffer;

	if (!Ptr_NtQuerySystemInformation)
	{
		return status;
	}

	if(*(PVOID *) buffer)
	{
		status = Ptr_NtQuerySystemInformation(informationClass, *(PVOID *) buffer, informationLength, NULL);
	}
	else
	{
		for(sizeOfBuffer = 0x1000; (status == STATUS_INFO_LENGTH_MISMATCH) && (*(PVOID *) buffer = LocalAlloc(LPTR, sizeOfBuffer)) ; sizeOfBuffer <<= 1)
		{
			status = Ptr_NtQuerySystemInformation(informationClass, *(PVOID *) buffer, sizeOfBuffer, NULL);
			if(!NT_SUCCESS(status))
				LocalFree(*(PVOID *) buffer);
		}
	}
	return status;
}

NTSTATUS kull_m_process_getProcessInformation(PKULL_M_PROCESS_ENUM_CALLBACK callBack, PVOID pvArg)
{
	NTSTATUS status;
	PSYSTEM_PROCESS_INFORMATION buffer = NULL, myInfos;

	status = kull_m_process_NtQuerySystemInformation(SystemProcessInformation, &buffer, 0);
	
	if(NT_SUCCESS(status))
	{
		for(myInfos = buffer; callBack(myInfos, pvArg) && myInfos->NextEntryOffset ; myInfos = (PSYSTEM_PROCESS_INFORMATION) ((PBYTE) myInfos + myInfos->NextEntryOffset));
		LocalFree(buffer);
	}
	return status;
}

BOOL CALLBACK kull_m_process_callback_pidForName(PSYSTEM_PROCESS_INFORMATION pSystemProcessInformation, PVOID pvArg)
{
	if (!Ptr_RtlEqualUnicodeString)
	{
		return FALSE;
	}
	if(((PKULL_M_PROCESS_PID_FOR_NAME) pvArg)->isFound = Ptr_RtlEqualUnicodeString(&pSystemProcessInformation->ImageName, ((PKULL_M_PROCESS_PID_FOR_NAME) pvArg)->name, TRUE))
		*((PKULL_M_PROCESS_PID_FOR_NAME) pvArg)->processId = PtrToUlong(pSystemProcessInformation->UniqueProcessId);
	return !((PKULL_M_PROCESS_PID_FOR_NAME) pvArg)->isFound;
}

BOOL kull_m_process_getProcessIdForName(LPCWSTR name, PDWORD processId)
{
	BOOL status = FALSE;
	UNICODE_STRING uName;
	KULL_M_PROCESS_PID_FOR_NAME mySearch = {&uName, processId, FALSE};
	if (!Ptr_RtlInitUnicodeString)
	{
		kprintf(L"Ptr_RtlInitUnicodeString is NULL.\n");
		return FALSE;
	}
	Ptr_RtlInitUnicodeString(&uName, name);
	if(NT_SUCCESS(kull_m_process_getProcessInformation(kull_m_process_callback_pidForName, &mySearch)))
		status = mySearch.isFound;
	return status;;
}

NTSTATUS kull_m_process_getVeryBasicModuleInformations(PKULL_M_MEMORY_HANDLE memory, PKULL_M_MODULE_ENUM_CALLBACK callBack, PVOID pvArg)
{
	NTSTATUS status = STATUS_DLL_NOT_FOUND;
	PLDR_DATA_TABLE_ENTRY pLdrEntry;
	PEB Peb; PEB_LDR_DATA LdrData; LDR_DATA_TABLE_ENTRY LdrEntry;
	PEB64 Peb64; PEB_LDR_DATA64 LdrData64; LDR_DATA_TABLE_ENTRY64 LdrEntry64;
#ifdef _M_X64
	PLDR_DATA_TABLE_ENTRY_F32 pLdrEntry32;
	PEB_F32 Peb32; PEB_LDR_DATA_F32 LdrData32; LDR_DATA_TABLE_ENTRY_F32 LdrEntry32;
#endif
	KULL_M_MEMORY_ADDRESS aBuffer = {NULL, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE};
	KULL_M_MEMORY_ADDRESS aProcess= {NULL, memory};
	PBYTE aLire, fin;
	DWORD64 aLire64, fin64;
	UNICODE_STRING moduleName;
	KULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION moduleInformation;
	PRTL_PROCESS_MODULES modules = NULL;
	BOOL continueCallback = TRUE;
	moduleInformation.DllBase.hMemory = memory;
	switch(memory->type)
	{
	case KULL_M_MEMORY_TYPE_OWN:
		if(kull_m_process_peb(memory, &Peb, FALSE))
		{
			for(pLdrEntry  = (PLDR_DATA_TABLE_ENTRY) ((PBYTE) (Peb.Ldr->InMemoryOrderModulevector.Flink) - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks));
				(pLdrEntry != (PLDR_DATA_TABLE_ENTRY) ((PBYTE) (Peb.Ldr) + FIELD_OFFSET(PEB_LDR_DATA, InLoadOrderModulevector))) && continueCallback;
				pLdrEntry  = (PLDR_DATA_TABLE_ENTRY) ((PBYTE) (pLdrEntry->InMemoryOrderLinks.Flink ) - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks))
					)
				{
					moduleInformation.DllBase.address = pLdrEntry->DllBase;
					moduleInformation.SizeOfImage = pLdrEntry->SizeOfImage;
					moduleInformation.NameDontUseOutsideCallback = &pLdrEntry->BaseDllName;
					kull_m_process_adjustTimeDateStamp(&moduleInformation);
					continueCallback = callBack(&moduleInformation, pvArg);
				}
				status = STATUS_SUCCESS;
		}
#ifdef _M_X64
		moduleInformation.NameDontUseOutsideCallback = &moduleName;
		if(continueCallback && NT_SUCCESS(status) && kull_m_process_peb(memory, (PPEB) &Peb32, TRUE))
		{
			status = STATUS_PARTIAL_COPY;
			
			for(pLdrEntry32  = (PLDR_DATA_TABLE_ENTRY_F32) ((PBYTE) ULongToPtr(((PEB_LDR_DATA_F32 *) ULongToPtr(Peb32.Ldr))->InMemoryOrderModulevector.Flink) - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY_F32, InMemoryOrderLinks));
				(pLdrEntry32 != (PLDR_DATA_TABLE_ENTRY_F32) ((PBYTE) ULongToPtr(Peb32.Ldr) + FIELD_OFFSET(PEB_LDR_DATA, InLoadOrderModulevector))) && continueCallback;
				pLdrEntry32  = (PLDR_DATA_TABLE_ENTRY_F32) ((PBYTE) ULongToPtr(pLdrEntry32->InMemoryOrderLinks.Flink) - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY_F32, InMemoryOrderLinks))
				)
			{
				moduleInformation.DllBase.address = ULongToPtr(pLdrEntry32->DllBase);
				moduleInformation.SizeOfImage = pLdrEntry32->SizeOfImage;
				moduleName.Length = pLdrEntry32->BaseDllName.Length;
				moduleName.MaximumLength = pLdrEntry32->BaseDllName.MaximumLength;
				moduleName.Buffer = (PWSTR) ULongToPtr(pLdrEntry32->BaseDllName.Buffer);
				kull_m_process_adjustTimeDateStamp(&moduleInformation);
				continueCallback = callBack(&moduleInformation, pvArg);
			}
			status = STATUS_SUCCESS;
		}
#endif
		break;

	case KULL_M_MEMORY_TYPE_PROCESS:
		moduleInformation.NameDontUseOutsideCallback = &moduleName;
		if(kull_m_process_peb(memory, &Peb, FALSE))
		{
			aBuffer.address = &LdrData; aProcess.address = Peb.Ldr;
			if(kull_m_memory_copy(&aBuffer, &aProcess, sizeof(LdrData)))
			{
				for(
					aLire  = (PBYTE) (LdrData.InMemoryOrderModulevector.Flink) - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks),
					fin    = (PBYTE) (Peb.Ldr) + FIELD_OFFSET(PEB_LDR_DATA, InLoadOrderModulevector);
					(aLire != fin) && continueCallback;
					aLire  = (PBYTE) LdrEntry.InMemoryOrderLinks.Flink - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks)
					)
				{
					aBuffer.address = &LdrEntry; aProcess.address = aLire;
					if(continueCallback = kull_m_memory_copy(&aBuffer, &aProcess, sizeof(LdrEntry)))
					{
						moduleInformation.DllBase.address = LdrEntry.DllBase;
						moduleInformation.SizeOfImage = LdrEntry.SizeOfImage;
						moduleName = LdrEntry.BaseDllName;
						if(moduleName.Buffer = (PWSTR) LocalAlloc(LPTR, moduleName.MaximumLength))
						{
							aBuffer.address = moduleName.Buffer; aProcess.address = LdrEntry.BaseDllName.Buffer;
							if(kull_m_memory_copy(&aBuffer, &aProcess, moduleName.MaximumLength))
							{
								kull_m_process_adjustTimeDateStamp(&moduleInformation);
								continueCallback = callBack(&moduleInformation, pvArg);
							}
							LocalFree(moduleName.Buffer);
						}
					}
				}
				status = STATUS_SUCCESS;
			}
		}
#ifdef _M_X64
		if(continueCallback && NT_SUCCESS(status) && kull_m_process_peb(memory, (PPEB) &Peb32, TRUE))
		{
			status = STATUS_PARTIAL_COPY;
			aBuffer.address = &LdrData32; aProcess.address = ULongToPtr(Peb32.Ldr);
			if(kull_m_memory_copy(&aBuffer, &aProcess, sizeof(LdrData32)))
			{
				for(
					aLire  = (PBYTE) ULongToPtr(LdrData32.InMemoryOrderModulevector.Flink) - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY_F32, InMemoryOrderLinks),
					fin    = (PBYTE) ULongToPtr(Peb32.Ldr) + FIELD_OFFSET(PEB_LDR_DATA_F32, InLoadOrderModulevector);
					(aLire != fin) && continueCallback;
					aLire  = (PBYTE) ULongToPtr(LdrEntry32.InMemoryOrderLinks.Flink) - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY_F32, InMemoryOrderLinks)
					)
				{
					aBuffer.address = &LdrEntry32; aProcess.address = aLire;
					if(kull_m_memory_copy(&aBuffer, &aProcess, sizeof(LdrEntry32)))
					{
						moduleInformation.DllBase.address = ULongToPtr(LdrEntry32.DllBase);
						moduleInformation.SizeOfImage = LdrEntry32.SizeOfImage;
						
						moduleName.Length = LdrEntry32.BaseDllName.Length;
						moduleName.MaximumLength = LdrEntry32.BaseDllName.MaximumLength;
						if(moduleName.Buffer = (PWSTR) LocalAlloc(LPTR, moduleName.MaximumLength))
						{
							aBuffer.address = moduleName.Buffer; aProcess.address = ULongToPtr(LdrEntry32.BaseDllName.Buffer);
							if(kull_m_memory_copy(&aBuffer, &aProcess, moduleName.MaximumLength))
							{
								kull_m_process_adjustTimeDateStamp(&moduleInformation);
								continueCallback = callBack(&moduleInformation, pvArg);
							}
							LocalFree(moduleName.Buffer);
						}
					}
				}
				status = STATUS_SUCCESS;
			}
		}
#endif
		break;
	case KULL_M_MEMORY_TYPE_PROCESS64:
		moduleInformation.NameDontUseOutsideCallback = &moduleName;
		if (kull_m_process_peb64(memory, &Peb64, FALSE))
		{
			aBuffer.address = &LdrData64; aProcess.address64 = Peb64.Ldr;
			if (kull_m_memory_copy(&aBuffer, &aProcess, sizeof(LdrData64)))
			{
				for (
					aLire64 = (LdrData64.InMemoryOrderModulevector.Flink) - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY64, InMemoryOrderLinks),
					fin64 = (Peb64.Ldr) + FIELD_OFFSET(PEB_LDR_DATA64, InLoadOrderModulevector);
					(aLire64 != fin64) && continueCallback;
					aLire64 = LdrEntry64.InMemoryOrderLinks.Flink - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY64, InMemoryOrderLinks)
					)
				{
					aBuffer.address = &LdrEntry64; aProcess.address64 = aLire64;
					if (continueCallback = kull_m_memory_copy(&aBuffer, &aProcess, sizeof(LdrEntry64)))
					{
						moduleInformation.DllBase.address = 0;
						moduleInformation.DllBase.address64 = LdrEntry64.DllBase;
						moduleInformation.SizeOfImage = LdrEntry64.SizeOfImage;
						moduleName.Length = LdrEntry64.BaseDllName.Length;
						moduleName.MaximumLength = LdrEntry64.BaseDllName.MaximumLength;
						if (moduleName.Buffer = (PWSTR)LocalAlloc(LPTR, moduleName.MaximumLength))
						{
							aBuffer.address = moduleName.Buffer; aProcess.address64 = LdrEntry64.BaseDllName.Buffer;
							if (kull_m_memory_copy(&aBuffer, &aProcess, moduleName.MaximumLength))
							{
								kull_m_process_adjustTimeDateStamp(&moduleInformation);
								continueCallback = callBack(&moduleInformation, pvArg);
							}
							LocalFree(moduleName.Buffer);
						}
					}
				}
				status = STATUS_SUCCESS;
			}
		}
		break;
	default:
		status = STATUS_NOT_IMPLEMENTED;
		break;
	}

	return status;
}

void kull_m_process_adjustTimeDateStamp(PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION information)
{
#if 0
	PIMAGE_NT_HEADERS ntHeaders;
	PIMAGE_NT_HEADERS64 ntHeaders64;
	if (IsWow64())
	{
		if (kull_m_process_ntheaders64(&information->DllBase, &ntHeaders64))
		{
			information->TimeDateStamp = ntHeaders64->FileHeader.TimeDateStamp;
			LocalFree(ntHeaders64);
		}
		else information->TimeDateStamp = 0;
	}
	else
	{
		if (kull_m_process_ntheaders(&information->DllBase, &ntHeaders))
		{
			information->TimeDateStamp = ntHeaders->FileHeader.TimeDateStamp;
			LocalFree(ntHeaders);
		}
		else information->TimeDateStamp = 0;
	}
#endif // 0

}

BOOL CALLBACK kull_m_process_callback_moduleForName(PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION pModuleInformation, PVOID pvArg)
{
	if (!Ptr_RtlEqualUnicodeString)
		return FALSE;
	if(((PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION_FOR_NAME) pvArg)->isFound = Ptr_RtlEqualUnicodeString(pModuleInformation->NameDontUseOutsideCallback, ((PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION_FOR_NAME) pvArg)->name, TRUE))
		*((PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION_FOR_NAME) pvArg)->informations = *pModuleInformation;
	return !((PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION_FOR_NAME) pvArg)->isFound;
}

BOOL CALLBACK kull_m_process_callback_moduleFirst(PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION pModuleInformation, PVOID pvArg)
{
	*(PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION) pvArg = *pModuleInformation;
	return FALSE;
}

BOOL kull_m_process_getVeryBasicModuleInformationsForName(PKULL_M_MEMORY_HANDLE memory, PCWSTR name, PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION informations)
{
	BOOL status = FALSE;
	UNICODE_STRING uName;
	KULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION_FOR_NAME mySearch = {&uName, informations, FALSE};

	if(name && Ptr_RtlInitUnicodeString)
	{
		Ptr_RtlInitUnicodeString(&uName, name);
		if(NT_SUCCESS(kull_m_process_getVeryBasicModuleInformations(memory, kull_m_process_callback_moduleForName, &mySearch)))
			status = mySearch.isFound;
	}
	else
		status = NT_SUCCESS(kull_m_process_getVeryBasicModuleInformations(memory, kull_m_process_callback_moduleFirst, informations));
	return status;
}

NTSTATUS kull_m_process_getMemoryInformations(PKULL_M_MEMORY_HANDLE memory, PKULL_M_MEMORY_RANGE_ENUM_CALLBACK callBack, PVOID pvArg)
{
	NTSTATUS status = STATUS_NOT_FOUND;
	MEMORY_BASIC_INFORMATION memoryInfos;
	PBYTE currentPage, maxPage;
	BOOL continueCallback = TRUE;

	if(!NT_SUCCESS(kull_m_process_NtQuerySystemInformation(KIWI_SystemMmSystemRangeStart, &maxPage, sizeof(PBYTE))))
		maxPage = MmSystemRangeStart;

	switch(memory->type)
	{
	case KULL_M_MEMORY_TYPE_OWN:
		for(currentPage = 0; (currentPage < maxPage) && continueCallback; currentPage += memoryInfos.RegionSize)
			if(VirtualQuery(currentPage, &memoryInfos, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION))
				continueCallback = callBack(&memoryInfos, pvArg);
			else break;
		status = STATUS_SUCCESS;
		break;
	case KULL_M_MEMORY_TYPE_PROCESS:
		for(currentPage = 0; (currentPage < maxPage) && continueCallback; currentPage += memoryInfos.RegionSize)
			if(VirtualQueryEx(memory->pHandleProcess->hProcess, currentPage, &memoryInfos, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION))
				continueCallback = callBack(&memoryInfos, pvArg);
			else break;
		status = STATUS_SUCCESS;
		break;
	default:
		break;
	}

	return status;
}

BOOL kull_m_process_peb(PKULL_M_MEMORY_HANDLE memory, PPEB pPeb, BOOL isWOW)
{
	BOOL status = FALSE;
	PROCESS_BASIC_INFORMATION processInformations;
	HANDLE hProcess = (memory->type == KULL_M_MEMORY_TYPE_PROCESS) ? memory->pHandleProcess->hProcess : GetCurrentProcess();
	KULL_M_MEMORY_ADDRESS aBuffer = {pPeb, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE};
	KULL_M_MEMORY_ADDRESS aProcess= {NULL, memory};
	PROCESSINFOCLASS info;
	ULONG szPeb, szBuffer, szInfos;
	LPVOID buffer;

#ifdef _M_X64
	if(isWOW)
	{
		info = ProcessWow64Information;
		szBuffer = sizeof(processInformations.PebBaseAddress);
		buffer = &processInformations.PebBaseAddress;
		szPeb = sizeof(PEB_F32);
	}
	else
	{
#endif
		info = ProcessBasicInformation;
		szBuffer = sizeof(processInformations);
		buffer = &processInformations;
		szPeb = sizeof(PEB);
#ifdef _M_X64
	}
#endif

	switch(memory->type)
	{
#ifndef MIMIKATZ_W2000_SUPPORT
	case KULL_M_MEMORY_TYPE_OWN:
		if(!isWOW)
		{
			if (Ptr_RtlGetCurrentPeb)
			{
				*pPeb = *Ptr_RtlGetCurrentPeb();
				status = TRUE;
			}
			break;
		}
#endif
	case KULL_M_MEMORY_TYPE_PROCESS:
		if(Ptr_NtQueryInformationProcess && NT_SUCCESS(Ptr_NtQueryInformationProcess(hProcess, info, buffer, szBuffer, &szInfos)) && (szInfos == szBuffer) && processInformations.PebBaseAddress)
		{
			aProcess.address = processInformations.PebBaseAddress;
			status = kull_m_memory_copy(&aBuffer, &aProcess, szPeb);
		}
		break;
	}
	return status;
}

BOOL kull_m_process_peb64(PKULL_M_MEMORY_HANDLE memory, PPEB64 pPeb, BOOL isWOW)
{
	BOOL status = FALSE;
	HANDLE hProcess = (memory->type == KULL_M_MEMORY_TYPE_PROCESS64) ? memory->pHandleProcess->hProcess : GetCurrentProcess();
	KULL_M_MEMORY_ADDRESS aBuffer = { pPeb, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE };
	KULL_M_MEMORY_ADDRESS aProcess = { NULL, memory, 0 };
	DWORD64 hNtdll64;
	PROCESS_BASIC_INFORMATION64 processInformations64;
	DWORD64 ptrNtQueryInformationProcess64;
	DWORD64 hProcess64 = (DWORD64)hProcess;
	DWORD64 info64 = ProcessBasicInformation;
	DWORD64 buffer64 = (DWORD64)&processInformations64;
	DWORD64 szBuffer64 = sizeof(processInformations64);
	DWORD64 szInfos64 = 0;
	LONG64 retVal64;
	ULONG szPeb64 = sizeof(PEB64);

	if (KULL_M_MEMORY_TYPE_PROCESS64 == memory->type)
	{
#if 0
		hNtdll64 = getNTDLL64();
		if (hNtdll64)
		{
			ptrNtQueryInformationProcess64 = GetProcAddress64(hNtdll64, "NtQueryInformationProcess");
			if (ptrNtQueryInformationProcess64)
			{
				retVal64 = X64Call(ptrNtQueryInformationProcess64, 5, hProcess64, info64, buffer64, szBuffer64, (DWORD64)&szInfos64);
				if (retVal64 >= 0)
				{
					aProcess.address64 = processInformations64.PebBaseAddress;
					status = kull_m_memory_copy(&aBuffer, &aProcess, szPeb64);
				}
			}
		}		hNtdll64 = getNTDLL64();
		if (hNtdll64)
		{
			ptrNtQueryInformationProcess64 = GetProcAddress64(hNtdll64, "NtQueryInformationProcess");
			if (ptrNtQueryInformationProcess64)
			{
				retVal64 = X64Call(ptrNtQueryInformationProcess64, 5, hProcess64, info64, buffer64, szBuffer64, (DWORD64)&szInfos64);
				if (retVal64 >= 0)
				{
					aProcess.address64 = processInformations64.PebBaseAddress;
					status = kull_m_memory_copy(&aBuffer, &aProcess, szPeb64);
				}
			}
		}
#endif
	}
	return status;
}

BOOL kull_m_process_ntheaders(PKULL_M_MEMORY_ADDRESS pBase, PIMAGE_NT_HEADERS * pHeaders)
{
	BOOL status = FALSE;
	IMAGE_DOS_HEADER headerImageDos;
	KULL_M_MEMORY_ADDRESS aBuffer = {&headerImageDos, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE}, aRealNtHeaders = {NULL, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE}, aProcess= {NULL, pBase->hMemory};
	DWORD size;

	if(kull_m_memory_copy(&aBuffer, pBase, sizeof(IMAGE_DOS_HEADER)) && headerImageDos.e_magic == IMAGE_DOS_SIGNATURE)
	{
		aProcess.address = (PBYTE)pBase->address + headerImageDos.e_lfanew;
		if (aBuffer.address = LocalAlloc(LPTR, sizeof(DWORD) + IMAGE_SIZEOF_FILE_HEADER))
		{
			if (kull_m_memory_copy(&aBuffer, &aProcess, sizeof(DWORD) + IMAGE_SIZEOF_FILE_HEADER) && ((PIMAGE_NT_HEADERS)aBuffer.address)->Signature == IMAGE_NT_SIGNATURE);
			{
				size = (((PIMAGE_NT_HEADERS)aBuffer.address)->FileHeader.Machine == IMAGE_FILE_MACHINE_I386) ? sizeof(IMAGE_NT_HEADERS32) : sizeof(IMAGE_NT_HEADERS64);
				if (aRealNtHeaders.address = (PIMAGE_NT_HEADERS)LocalAlloc(LPTR, size))
				{
					status = kull_m_memory_copy(&aRealNtHeaders, &aProcess, size);

					if (status)
						*pHeaders = (PIMAGE_NT_HEADERS)aRealNtHeaders.address;
					else
						LocalFree(aRealNtHeaders.address);
				}
			}
			LocalFree(aBuffer.address);
		}
	}
	return status;
}

BOOL kull_m_process_ntheaders64(PKULL_M_MEMORY_ADDRESS pBase, PIMAGE_NT_HEADERS64 * pHeaders)
{
	BOOL status = FALSE;
	IMAGE_DOS_HEADER headerImageDos;
	KULL_M_MEMORY_ADDRESS aBuffer = { &headerImageDos, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE }, aRealNtHeaders = { NULL, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE }, aProcess = { NULL, pBase->hMemory };
	DWORD size;

	if (kull_m_memory_copy(&aBuffer, pBase, sizeof(IMAGE_DOS_HEADER)) && headerImageDos.e_magic == IMAGE_DOS_SIGNATURE)
	{
		aProcess.address64 = pBase->address64 + headerImageDos.e_lfanew;
		if (aBuffer.address = LocalAlloc(LPTR, sizeof(DWORD) + IMAGE_SIZEOF_FILE_HEADER))
		{
			if (kull_m_memory_copy(&aBuffer, &aProcess, sizeof(DWORD) + IMAGE_SIZEOF_FILE_HEADER) && ((PIMAGE_NT_HEADERS64)aBuffer.address)->Signature == IMAGE_NT_SIGNATURE);
			{
				size = (((PIMAGE_NT_HEADERS64)aBuffer.address)->FileHeader.Machine == IMAGE_FILE_MACHINE_I386) ? sizeof(IMAGE_NT_HEADERS32) : sizeof(IMAGE_NT_HEADERS64);
				if (aRealNtHeaders.address = (PIMAGE_NT_HEADERS64)LocalAlloc(LPTR, size))
				{
					status = kull_m_memory_copy(&aRealNtHeaders, &aProcess, size);

					if (status)
						*pHeaders = (PIMAGE_NT_HEADERS64)aRealNtHeaders.address;
					else
						LocalFree(aRealNtHeaders.address);
				}
				LocalFree(aBuffer.address);
			}
		}
	}
	return status;
}

BOOL kull_m_process_datadirectory(PKULL_M_MEMORY_ADDRESS pBase, DWORD entry, PDWORD pRva, PDWORD pSize, PWORD pMachine, PVOID *pData)
{
	BOOL status = FALSE;
	KULL_M_MEMORY_ADDRESS aBuffer = {NULL, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE};
	KULL_M_MEMORY_ADDRESS aProcess= *pBase;
	
	DWORD rva, size;

	PIMAGE_NT_HEADERS pNtHeaders;
	if(kull_m_process_ntheaders(pBase, &pNtHeaders))
	{
		if(pMachine)
			*pMachine = pNtHeaders->FileHeader.Machine;
		
		if(pNtHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
		{
			rva = ((PIMAGE_NT_HEADERS32) pNtHeaders)->OptionalHeader.DataDirectory[entry].VirtualAddress;
			size = ((PIMAGE_NT_HEADERS32) pNtHeaders)->OptionalHeader.DataDirectory[entry].Size;
		}
		else
		{
			rva = ((PIMAGE_NT_HEADERS64) pNtHeaders)->OptionalHeader.DataDirectory[entry].VirtualAddress;
			size = ((PIMAGE_NT_HEADERS64) pNtHeaders)->OptionalHeader.DataDirectory[entry].Size;
		}
		
		if(pRva)
			*pRva = rva;
		if(pSize)
			*pSize = size;

		if(rva && size && pData)
		{
			if(*pData = LocalAlloc(LPTR, size))
			{
				aProcess.address = (PBYTE) pBase->address + rva;
				aBuffer.address = *pData;
				status = kull_m_memory_copy(&aBuffer, &aProcess, size);

				if(!status)
					LocalFree(*pData);
			}
		}
		LocalFree(pNtHeaders);
	}
	return status;
}

BOOL kull_m_process_create(KULL_M_PROCESS_CREATE_TYPE type, PCWSTR commandLine, DWORD processFlags, HANDLE hUserToken, DWORD logonFlags, PCWSTR user, PCWSTR domain, PCWSTR password, PPROCESS_INFORMATION pProcessInfos, BOOL autoCloseHandle)
{
	BOOL status = FALSE;
	DWORD iProcessFlags = CREATE_NEW_CONSOLE | processFlags;
	DWORD iLogonFlags = 0 | logonFlags;
	PWSTR dupCommandLine;
	PPROCESS_INFORMATION ptrProcessInfos;
	STARTUPINFO startupInfo;
	RtlZeroMemory(&startupInfo, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);

	ptrProcessInfos = pProcessInfos ? pProcessInfos : (PPROCESS_INFORMATION) LocalAlloc(LPTR, sizeof(PROCESS_INFORMATION));

	if(dupCommandLine = _wcsdup(commandLine))
	{
		switch(type)
		{
		case KULL_M_PROCESS_CREATE_NORMAL:
			status = CreateProcess(NULL, dupCommandLine, NULL, NULL, FALSE, iProcessFlags, NULL, NULL, &startupInfo, ptrProcessInfos);
			break;
		case KULL_M_PROCESS_CREATE_USER:
			status = CreateProcessAsUser(hUserToken, NULL, dupCommandLine, NULL, NULL, FALSE, iProcessFlags, NULL, NULL, &startupInfo, ptrProcessInfos);
			break;
		/*case KULL_M_PROCESS_CREATE_TOKEN:
			status = CreateProcessWithTokenW(hUserToken, iLogonFlags, NULL, dupCommandLine, iProcessFlags, NULL, NULL, &startupInfo, ptrProcessInfos);
			break;*/
		case KULL_M_PROCESS_CREATE_LOGON:
			status = CreateProcessWithLogonW(user, domain, password, iLogonFlags, NULL, dupCommandLine, iProcessFlags, NULL, NULL, &startupInfo, ptrProcessInfos);
			break;
		}

		if(autoCloseHandle || !pProcessInfos)
		{
			CloseHandle(ptrProcessInfos->hThread);
			CloseHandle(ptrProcessInfos->hProcess);
		}

		if(!pProcessInfos)
			LocalFree(ptrProcessInfos);
		free(dupCommandLine);
	}
	return status;
}

NTSTATUS kull_m_process_getExportedEntryInformations(PKULL_M_MEMORY_ADDRESS address, PKULL_M_EXPORTED_ENTRY_ENUM_CALLBACK callBack, PVOID pvArg)
{
	PIMAGE_EXPORT_DIRECTORY pExportDir;
	DWORD rva, size, i, j, rvaFunc;
	KULL_M_PROCESS_EXPORTED_ENTRY exportedEntry;
	BOOL continueCallback = TRUE;

	if(kull_m_process_datadirectory(address, IMAGE_DIRECTORY_ENTRY_EXPORT, &rva, &size, &exportedEntry.machine, (PVOID *) &pExportDir))
	{
		exportedEntry.function.hMemory = exportedEntry.pRva.hMemory = address->hMemory;
		for(i = 0; (i < pExportDir->NumberOfFunctions) && continueCallback ; i++)
		{
			if(rvaFunc = ((PDWORD) ((PBYTE) pExportDir + (pExportDir->AddressOfFunctions - rva)))[i])
			{
				exportedEntry.pRva.address = (PBYTE) address->address + pExportDir->AddressOfFunctions + (i * sizeof(DWORD));
				exportedEntry.ordinal = i + 1;
				for(j = 0, exportedEntry.hint = 0, exportedEntry.name = NULL; (j < pExportDir->NumberOfNames) && !exportedEntry.name && continueCallback; j++)
				{
					if(i == ((PWORD) ((PBYTE) pExportDir + (pExportDir->AddressOfNameOrdinals - rva)))[j])
					{
						exportedEntry.name = (PSTR) ((PBYTE) pExportDir + (((PDWORD) ((PBYTE) pExportDir + (pExportDir->AddressOfNames - rva)))[j] - rva));
						exportedEntry.hint = j;
					}
				}

				if((rvaFunc < rva) || (rvaFunc >= (rva + size)))
				{
					exportedEntry.function.address = (PBYTE) address->address + rvaFunc;
					exportedEntry.redirect = NULL;
				}
				else
				{
					exportedEntry.function.address = NULL;
					exportedEntry.redirect = (PSTR) ((PBYTE) pExportDir + (rvaFunc - rva));
				}

				continueCallback = callBack(&exportedEntry, pvArg);
			}
		}
		LocalFree(pExportDir);
	}
	return STATUS_SUCCESS;
}

PSTR kull_m_process_getImportNameWithoutEnd(PKULL_M_MEMORY_ADDRESS base)
{
	CHAR sEnd = '\0';
	SIZE_T size;
	KULL_M_MEMORY_ADDRESS aStringBuffer = {NULL, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE}, aNullBuffer = {&sEnd, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE};
	KULL_M_MEMORY_SEARCH sMemory = {{{base->address, base->hMemory}, MAX_PATH}, NULL};

	if(kull_m_memory_search(&aNullBuffer, sizeof(sEnd), &sMemory, FALSE))
	{
		size = (PBYTE) sMemory.result - (PBYTE) base->address + sizeof(char);
		if(aStringBuffer.address = LocalAlloc(LPTR, size))
			if(!kull_m_memory_copy(&aStringBuffer, base, size))
				aStringBuffer.address = LocalFree(aStringBuffer.address);
	}
	return (PSTR) aStringBuffer.address;
}

NTSTATUS kull_m_process_getImportedEntryInformations(PKULL_M_MEMORY_ADDRESS address, PKULL_M_IMPORTED_ENTRY_ENUM_CALLBACK callBack, PVOID pvArg)
{
	PVOID pLocalBuffer;
	PIMAGE_IMPORT_DESCRIPTOR pImportDir;
	ULONG sizeThunk;
	ULONGLONG OriginalFirstThunk, FirstThunk, ordinalPattern;
	KULL_M_MEMORY_ADDRESS aOriginalFirstThunk = {&OriginalFirstThunk, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE}, aFirstThunk = {&FirstThunk, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE};
	KULL_M_MEMORY_ADDRESS aProcOriginalFirstThunk = {NULL, address->hMemory}, aProcName = {NULL, address->hMemory};
	KULL_M_PROCESS_IMPORTED_ENTRY importedEntry;
	BOOL continueCallback = TRUE;

	importedEntry.pFunction.hMemory = address->hMemory;
	importedEntry.function.hMemory = address->hMemory;

	if(kull_m_process_datadirectory(address, IMAGE_DIRECTORY_ENTRY_IMPORT, NULL, NULL, &importedEntry.machine, &pLocalBuffer))
	{
		if(importedEntry.machine == IMAGE_FILE_MACHINE_I386)
		{
			sizeThunk = sizeof(IMAGE_THUNK_DATA32);
			ordinalPattern = IMAGE_ORDINAL_FLAG32;
		}
		else
		{
			sizeThunk = sizeof(IMAGE_THUNK_DATA64);
			ordinalPattern = IMAGE_ORDINAL_FLAG64;
		}
		
		for(pImportDir = (PIMAGE_IMPORT_DESCRIPTOR) pLocalBuffer ; pImportDir->Characteristics && continueCallback; pImportDir++)
		{
			aProcName.address = (PBYTE) address->address + pImportDir->Name;
			if(importedEntry.libname = kull_m_process_getImportNameWithoutEnd(&aProcName))
			{
				for(
					aProcOriginalFirstThunk.address = ((PBYTE) address->address + pImportDir->OriginalFirstThunk),
					importedEntry.pFunction.address = ((PBYTE) address->address + pImportDir->FirstThunk);

					(kull_m_memory_copy(&aOriginalFirstThunk, &aProcOriginalFirstThunk, sizeThunk) && kull_m_memory_copy(&aFirstThunk, &importedEntry.pFunction, sizeThunk)) && (OriginalFirstThunk && FirstThunk) ;

					aProcOriginalFirstThunk.address = ((PBYTE) aProcOriginalFirstThunk.address + sizeThunk), ((PDWORD) &OriginalFirstThunk)[1] = 0,
					importedEntry.pFunction.address = ((PBYTE) importedEntry.pFunction.address + sizeThunk), ((PDWORD) &FirstThunk)[1] = 0
					)
				{
					importedEntry.function.address = (PVOID) FirstThunk;
					if(OriginalFirstThunk & ordinalPattern)
					{
						importedEntry.name = NULL;
						importedEntry.ordinal = IMAGE_ORDINAL(OriginalFirstThunk);
					}
					else
					{
						aProcName.address = ((PIMAGE_IMPORT_BY_NAME) ((PBYTE) address->address + OriginalFirstThunk))->Name;
						importedEntry.name = kull_m_process_getImportNameWithoutEnd(&aProcName);
						importedEntry.ordinal = 0;
					}

					continueCallback = callBack(&importedEntry, pvArg);

					if(importedEntry.name)
						LocalFree(importedEntry.name);

				}
				LocalFree(importedEntry.libname);
			}
		}
		LocalFree(pLocalBuffer);
	}
	return TRUE;
}

BOOL kull_m_process_getUnicodeString(IN PUNICODE_STRING string, IN PKULL_M_MEMORY_HANDLE source)
{
	BOOL status = FALSE;
	KULL_M_MEMORY_HANDLE hOwn = {KULL_M_MEMORY_TYPE_OWN, NULL};
	KULL_M_MEMORY_ADDRESS aDestin = {NULL, &hOwn};
	KULL_M_MEMORY_ADDRESS aSource = {string->Buffer, source};
	
	string->Buffer = NULL;
	if(aSource.address && string->MaximumLength)
	{
		if(aDestin.address = LocalAlloc(LPTR, string->MaximumLength))
		{
			string->Buffer = (PWSTR) aDestin.address;
			status = kull_m_memory_copy(&aDestin, &aSource, string->MaximumLength);
		}
	}
	return status;
}

BOOL kull_m_process_getUnicodeStringSingle64(IN PLSA_UNICODE_STRING64 string, IN PKULL_M_MEMORY_HANDLE source)
{
	BOOL status = FALSE;
	KULL_M_MEMORY_HANDLE hOwn = { KULL_M_MEMORY_TYPE_OWN, NULL };
	KULL_M_MEMORY_ADDRESS aDestin = { NULL, &hOwn };
	KULL_M_MEMORY_ADDRESS aSource = { NULL, source, string->Buffer };

	string->Buffer = 0;
	if (aSource.address64 && string->MaximumLength)
	{
		if (aDestin.address = LocalAlloc(LPTR, string->MaximumLength))
		{
			string->Buffer = (DWORD64)aDestin.address;
			status = kull_m_memory_copy(&aDestin, &aSource, string->MaximumLength);
		}
	}
	return status;
}

BOOL kull_m_process_getUnicodeString64(OUT PLSA_UNICODE_STRING string, IN PLSA_UNICODE_STRING64 srcstring, IN PKULL_M_MEMORY_HANDLE source)
{
	BOOL status = FALSE;
	KULL_M_MEMORY_HANDLE hOwn = { KULL_M_MEMORY_TYPE_OWN, NULL };
	KULL_M_MEMORY_ADDRESS aDestin = { NULL, &hOwn };
	KULL_M_MEMORY_ADDRESS aSource = { NULL, source, srcstring->Buffer };
	
	string->Length = srcstring->Length;
	string->MaximumLength = srcstring->MaximumLength;
	string->Buffer = 0;
	if (aSource.address64 && string->MaximumLength)
	{
		if (aDestin.address = LocalAlloc(LPTR, string->MaximumLength))
		{
			string->Buffer = (PWSTR)aDestin.address;
			status = kull_m_memory_copy(&aDestin, &aSource, string->MaximumLength);
		}
	}
	return status;
}

BOOL kull_m_process_getSid(IN PSID * pSid, IN PKULL_M_MEMORY_HANDLE source)
{
	BOOL status = FALSE;
	BYTE nbAuth;
	DWORD sizeSid;
	KULL_M_MEMORY_HANDLE hOwn = {KULL_M_MEMORY_TYPE_OWN, NULL};
	KULL_M_MEMORY_ADDRESS aDestin = {&nbAuth, &hOwn};
	KULL_M_MEMORY_ADDRESS aSource = {(PBYTE) *pSid + 1, source};

	*pSid = NULL;
	if(kull_m_memory_copy(&aDestin, &aSource, sizeof(BYTE)))
	{
		aSource.address = (PBYTE) aSource.address - 1;
		sizeSid =  4 * nbAuth + 6 + 1 + 1;

		if(aDestin.address = LocalAlloc(LPTR, sizeSid))
		{
			*pSid = (PSID) aDestin.address;
			status = kull_m_memory_copy(&aDestin, &aSource, sizeSid);
		}
	}
	return status;
}

BOOL kull_m_process_getSid64(IN PSID * pSid, IN DWORD64 pSidAdress, IN PKULL_M_MEMORY_HANDLE source)
{
	BOOL status = FALSE;
	BYTE nbAuth;
	DWORD sizeSid;
	KULL_M_MEMORY_HANDLE hOwn = { KULL_M_MEMORY_TYPE_OWN, NULL };
	KULL_M_MEMORY_ADDRESS aDestin = { &nbAuth, &hOwn };
	KULL_M_MEMORY_ADDRESS aSource = { NULL/*(PBYTE)*pSid + 1*/, source , pSidAdress + sizeof(BYTE)};

	*pSid = NULL;
	if (kull_m_memory_copy(&aDestin, &aSource, sizeof(BYTE)))
	{
		aSource.address64 = pSidAdress;
		sizeSid = 4 * nbAuth + 6 + 1 + 1;

		if (aDestin.address = LocalAlloc(LPTR, sizeSid))
		{
			*pSid = (PSID)aDestin.address;
			status = kull_m_memory_copy(&aDestin, &aSource, sizeSid);
		}
	}
	return status;
}