/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kull_m_handle.h"

NTSTATUS kull_m_handle_getHandles(PKULL_M_SYSTEM_HANDLE_ENUM_CALLBACK callBack, PVOID pvArg)
{
#if 0
	NTSTATUS status;
	ULONG i;
	PSYSTEM_HANDLE_INFORMATION buffer = NULL;

	status = kull_m_process_NtQuerySystemInformation(SystemHandleInformation, &buffer, 0);
	if (NT_SUCCESS(status))
	{
		for (i = 0; (i < buffer->HandleCount) && callBack(&buffer->Handles[i], pvArg); i++);
		LocalFree(buffer);
	}
	return status;
#else
	return -1;
#endif // 0

}

BOOL kull_m_handle_GetUserObjectInformation(HANDLE hObj, int nIndex, PVOID *pvInfo, PDWORD nLength)
{
	BOOL status = FALSE;
	DWORD szNeeded;

	if(!GetUserObjectInformation(hObj, nIndex, NULL, 0, &szNeeded) && (GetLastError() == ERROR_INSUFFICIENT_BUFFER) && szNeeded)
	{
		if(*pvInfo = LocalAlloc(LPTR, szNeeded))
		{
			if(nLength)
				*nLength = szNeeded;
			if(!(status = GetUserObjectInformation(hObj, nIndex, *pvInfo, szNeeded, &szNeeded)))
				LocalFree(*pvInfo);
		}
	}
	return status;
}