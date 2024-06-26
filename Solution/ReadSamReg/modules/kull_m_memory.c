/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kull_m_memory.h"

KULL_M_MEMORY_HANDLE KULL_M_MEMORY_GLOBAL_OWN_HANDLE = {KULL_M_MEMORY_TYPE_OWN, NULL};

BOOL kull_m_memory_open(IN KULL_M_MEMORY_TYPE Type, IN HANDLE hAny, OUT PKULL_M_MEMORY_HANDLE *hMemory)
{
	BOOL status = FALSE;

	*hMemory = (PKULL_M_MEMORY_HANDLE) LocalAlloc(LPTR, sizeof(KULL_M_MEMORY_HANDLE));
	if(*hMemory)
	{
		(*hMemory)->type = Type;
		switch (Type)
		{
		case KULL_M_MEMORY_TYPE_OWN:
			status = TRUE;
			break;
		case KULL_M_MEMORY_TYPE_PROCESS:
		case KULL_M_MEMORY_TYPE_PROCESS64:
			if((*hMemory)->pHandleProcess = (PKULL_M_MEMORY_HANDLE_PROCESS) LocalAlloc(LPTR, sizeof(KULL_M_MEMORY_HANDLE_PROCESS)))
			{
				(*hMemory)->pHandleProcess->hProcess = hAny;
				status = TRUE;
			}
			break;
		case KULL_M_MEMORY_TYPE_FILE:
			if((*hMemory)->pHandleFile = (PKULL_M_MEMORY_HANDLE_FILE) LocalAlloc(LPTR, sizeof(KULL_M_MEMORY_HANDLE_FILE)))
			{
				(*hMemory)->pHandleFile->hFile = hAny;
				status = TRUE;
			}
			break;
		default:
			break;
		}
		if(!status)
			LocalFree(*hMemory);
	}
	return status;
}

PKULL_M_MEMORY_HANDLE kull_m_memory_close(IN PKULL_M_MEMORY_HANDLE hMemory)
{
	if(hMemory)
	{
		switch (hMemory->type)
		{
		case KULL_M_MEMORY_TYPE_PROCESS:
		case KULL_M_MEMORY_TYPE_PROCESS64:
			LocalFree(hMemory->pHandleProcess);
			break;
		case KULL_M_MEMORY_TYPE_FILE:
			LocalFree(hMemory->pHandleFile);
			break;
		default:
			break;
		}
		return (PKULL_M_MEMORY_HANDLE) LocalFree(hMemory);
	}
	else return NULL;
}

BOOL kull_m_memory_copy(OUT PKULL_M_MEMORY_ADDRESS Destination, IN PKULL_M_MEMORY_ADDRESS Source, IN SIZE_T Length)
{
	BOOL status = FALSE;
	BOOL bufferMeFirst = FALSE;
	KULL_M_MEMORY_ADDRESS aBuffer = {NULL, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE};
	DWORD nbReadWrite;

	switch(Destination->hMemory->type)
	{
	case KULL_M_MEMORY_TYPE_OWN:
		switch(Source->hMemory->type)
		{
		case KULL_M_MEMORY_TYPE_OWN:
			RtlCopyMemory(Destination->address, Source->address, Length);
			status = TRUE;
			break;
		case KULL_M_MEMORY_TYPE_PROCESS:
			status = ReadProcessMemory(Source->hMemory->pHandleProcess->hProcess, Source->address, Destination->address, Length, NULL);
			break;
		case KULL_M_MEMORY_TYPE_PROCESS64:
			// status = ReadProcessMemory64(Source->hMemory->pHandleProcess->hProcess, Source->address64, Destination->address, Length, NULL);
			break;
		case KULL_M_MEMORY_TYPE_FILE:
			if(SetFilePointer(Source->hMemory->pHandleFile->hFile, PtrToLong(Source->address), NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
				status = ReadFile(Source->hMemory->pHandleFile->hFile, Destination->address, (DWORD) Length, &nbReadWrite, NULL);
			break;
		default:
			break;
		}
		break;
	case KULL_M_MEMORY_TYPE_PROCESS:
		switch(Source->hMemory->type)
		{
		case KULL_M_MEMORY_TYPE_OWN:
			status = WriteProcessMemory(Destination->hMemory->pHandleProcess->hProcess, Destination->address, Source->address, Length, NULL);
			break;
		default:
			bufferMeFirst = TRUE;
			break;
		}
		break;
	case KULL_M_MEMORY_TYPE_FILE:
		switch(Source->hMemory->type)
		{
		case KULL_M_MEMORY_TYPE_OWN:
			if(!Destination->address || SetFilePointer(Destination->hMemory->pHandleFile->hFile, PtrToLong(Destination->address), NULL, FILE_BEGIN))
				status = WriteFile(Destination->hMemory->pHandleFile->hFile, Source->address, (DWORD) Length, &nbReadWrite, NULL);
			break;
		default:
			bufferMeFirst = TRUE;
			break;
		}
		break;
	default:
		break;
	}

	if(bufferMeFirst)
	{
		if(aBuffer.address = LocalAlloc(LPTR, Length))
		{
			if(kull_m_memory_copy(&aBuffer, Source, Length))
				status = kull_m_memory_copy(Destination, &aBuffer, Length);
			LocalFree(aBuffer.address);
		}
	}
	return status;
}

BOOL kull_m_memory_search(IN PKULL_M_MEMORY_ADDRESS Pattern, IN SIZE_T Length, IN PKULL_M_MEMORY_SEARCH Search, IN BOOL bufferMeFirst)
{
	BOOL status = FALSE;
	KULL_M_MEMORY_SEARCH  sBuffer = {{{NULL, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE}, Search->kull_m_memoryRange.size}, NULL};
	PBYTE CurrentPtr = NULL;
	PBYTE limite = (PBYTE) Search->kull_m_memoryRange.kull_m_memoryAdress.address + Search->kull_m_memoryRange.size;

	switch(Pattern->hMemory->type)
	{
	case KULL_M_MEMORY_TYPE_OWN:
		switch(Search->kull_m_memoryRange.kull_m_memoryAdress.hMemory->type)
		{
		case KULL_M_MEMORY_TYPE_OWN:
			for(CurrentPtr = (PBYTE) Search->kull_m_memoryRange.kull_m_memoryAdress.address; !status && (CurrentPtr + Length <= limite); CurrentPtr++)
				status = RtlEqualMemory(Pattern->address, CurrentPtr, Length);
			CurrentPtr--;
			break;
		case KULL_M_MEMORY_TYPE_PROCESS:
		case KULL_M_MEMORY_TYPE_FILE:
			if(sBuffer.kull_m_memoryRange.kull_m_memoryAdress.address = LocalAlloc(LPTR, Search->kull_m_memoryRange.size))
			{
				if(kull_m_memory_copy(&sBuffer.kull_m_memoryRange.kull_m_memoryAdress, &Search->kull_m_memoryRange.kull_m_memoryAdress, Search->kull_m_memoryRange.size))
					if(status = kull_m_memory_search(Pattern, Length, &sBuffer, FALSE))
						CurrentPtr = (PBYTE) Search->kull_m_memoryRange.kull_m_memoryAdress.address + (((PBYTE) sBuffer.result) - (PBYTE) sBuffer.kull_m_memoryRange.kull_m_memoryAdress.address);
				LocalFree(sBuffer.kull_m_memoryRange.kull_m_memoryAdress.address);
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	Search->result = status ? CurrentPtr : NULL;

	return status;
}

BOOL kull_m_memory_search64(IN PKULL_M_MEMORY_ADDRESS Pattern, IN SIZE_T Length, IN PKULL_M_MEMORY_SEARCH Search, IN BOOL bufferMeFirst)
{
	BOOL status = FALSE;
	KULL_M_MEMORY_SEARCH  sBuffer = { { { NULL, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE }, Search->kull_m_memoryRange.size }, NULL, 0 };
	DWORD64 CurrentPtr = 0;
	DWORD64 limite;

	switch (Pattern->hMemory->type)
	{
	case KULL_M_MEMORY_TYPE_OWN:
		switch (Search->kull_m_memoryRange.kull_m_memoryAdress.hMemory->type)
		{
		case KULL_M_MEMORY_TYPE_OWN:
			limite = (DWORD64)((PBYTE)Search->kull_m_memoryRange.kull_m_memoryAdress.address + Search->kull_m_memoryRange.size);
			for (CurrentPtr = (DWORD64)Search->kull_m_memoryRange.kull_m_memoryAdress.address; !status && (CurrentPtr + Length <= limite); CurrentPtr++)
				status = RtlEqualMemory(Pattern->address, (LPBYTE)CurrentPtr, Length);
			CurrentPtr--;
			break;
		case KULL_M_MEMORY_TYPE_PROCESS:
		case KULL_M_MEMORY_TYPE_PROCESS64:
		case KULL_M_MEMORY_TYPE_FILE:
			if (sBuffer.kull_m_memoryRange.kull_m_memoryAdress.address = LocalAlloc(LPTR, Search->kull_m_memoryRange.size))
			{
				if (kull_m_memory_copy(&sBuffer.kull_m_memoryRange.kull_m_memoryAdress, &Search->kull_m_memoryRange.kull_m_memoryAdress, Search->kull_m_memoryRange.size))
					if (status = kull_m_memory_search64(Pattern, Length, &sBuffer, FALSE))
						CurrentPtr = Search->kull_m_memoryRange.kull_m_memoryAdress.address64 + (((PBYTE)sBuffer.result64) - (PBYTE)sBuffer.kull_m_memoryRange.kull_m_memoryAdress.address);
				LocalFree(sBuffer.kull_m_memoryRange.kull_m_memoryAdress.address);
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	Search->result64 = status ? (DWORD64)CurrentPtr : 0;

	return status;
}

BOOL kull_m_memory_alloc(IN PKULL_M_MEMORY_ADDRESS Address, IN SIZE_T Lenght, IN DWORD Protection)
{
	PVOID ptrAddress = &Address->address;
	DWORD lenPtr = sizeof(PVOID);
	Address->address = NULL;
	switch(Address->hMemory->type)
	{
		case KULL_M_MEMORY_TYPE_OWN:
			Address->address = VirtualAlloc(NULL, Lenght, MEM_COMMIT, Protection);
			break;
		case KULL_M_MEMORY_TYPE_PROCESS:
			Address->address = VirtualAllocEx(Address->hMemory->pHandleProcess->hProcess, NULL, Lenght, MEM_COMMIT, Protection);
			break;
		default:
			break;
	}
	return (Address->address) != NULL;
}

BOOL kull_m_memory_free(IN PKULL_M_MEMORY_ADDRESS Address, IN SIZE_T Lenght)
{
	BOOL status = FALSE;

	switch(Address->hMemory->type)
	{
		case KULL_M_MEMORY_TYPE_OWN:
			status = VirtualFree(Address->address, Lenght, MEM_RELEASE);
			break;
		case KULL_M_MEMORY_TYPE_PROCESS:
			status = VirtualFreeEx(Address->hMemory->pHandleProcess->hProcess, Address->address, Lenght, MEM_RELEASE);
			break;
		default:
			break;
	}
	return status;
}


BOOL kull_m_memory_query(IN PKULL_M_MEMORY_ADDRESS Address, OUT PMEMORY_BASIC_INFORMATION MemoryInfo)
{
	BOOL status = FALSE;

	switch(Address->hMemory->type)
	{
	case KULL_M_MEMORY_TYPE_OWN:
		status = VirtualQuery(Address->address, MemoryInfo, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION);
		break;
	case KULL_M_MEMORY_TYPE_PROCESS:
		status = VirtualQueryEx(Address->hMemory->pHandleProcess->hProcess, Address->address, MemoryInfo, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION);
		break;
	default:
		break;
	}

	return status;
}

BOOL kull_m_memory_protect(IN PKULL_M_MEMORY_ADDRESS Address, IN SIZE_T dwSize, IN DWORD flNewProtect, OUT OPTIONAL PDWORD lpflOldProtect)
{
	BOOL status = FALSE;
	DWORD OldProtect;

	switch(Address->hMemory->type)
	{
	case KULL_M_MEMORY_TYPE_OWN:
		status = VirtualProtect(Address->address, dwSize, flNewProtect, &OldProtect);
		break;
	case KULL_M_MEMORY_TYPE_PROCESS:
		status = VirtualProtectEx(Address->hMemory->pHandleProcess->hProcess, Address->address, dwSize, flNewProtect, &OldProtect);
		break;
	default:
		break;
	}

	if(status && lpflOldProtect)
		*lpflOldProtect = OldProtect;

	return status;
}

BOOL kull_m_memory_equal(IN PKULL_M_MEMORY_ADDRESS Address1, IN PKULL_M_MEMORY_ADDRESS Address2, IN SIZE_T Lenght)
{
	BOOL status = FALSE;
	KULL_M_MEMORY_ADDRESS aBuffer = {NULL, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE};
	switch(Address1->hMemory->type)
	{
	case KULL_M_MEMORY_TYPE_OWN:
		switch(Address2->hMemory->type)
		{
		case KULL_M_MEMORY_TYPE_OWN:
			status = RtlEqualMemory(Address1->address, Address2->address, Lenght);
			break;
		default:
			status = kull_m_memory_equal(Address2, Address1, Lenght);
			break;
		}
		break;
	default:
		if(aBuffer.address = LocalAlloc(LPTR, Lenght))
		{
			if(kull_m_memory_copy(&aBuffer, Address1, Lenght))
				status = kull_m_memory_equal(&aBuffer, Address2, Lenght);
			LocalFree(aBuffer.address);
		}
		break;
	}
	return status;
}