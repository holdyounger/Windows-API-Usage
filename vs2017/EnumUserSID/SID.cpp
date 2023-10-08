#include <winternl.h>
#ifndef _NTDEF_
#define _NTDEF_
#include <Ntsecapi.h>
#undef _NTDEF_
#else
#include <Ntsecapi.h>
#endif

#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004

typedef LONG KPRIORITY;

typedef struct _REAL_SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER SpareLi1;
	LARGE_INTEGER SpareLi2;
	LARGE_INTEGER SpareLi3;
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	KPRIORITY BasePriority;
	HANDLE UniqueProcessId;
	HANDLE InheritedFromUniqueProcessId;
	ULONG HandleCount;
	ULONG SessionId;
	ULONG_PTR PageDirectoryBase;
	SIZE_T PeakVirtualSize;
	ULONG VirtualSize;
	SIZE_T PageFaultCount;
	SIZE_T PeakWorkingSetSize;
	SIZE_T WorkingSetSize;
	SIZE_T QuotaPeakPagedPoolUsage;
	SIZE_T QuotaPagedPoolUsage;
	SIZE_T QuotaPeakNonPagedPoolUsage;
	SIZE_T QuotaNonPagedPoolUsage;
	SIZE_T PagefileUsage;
	SIZE_T PeakPagefileUsage;
	SIZE_T PrivatePageCount;
	LARGE_INTEGER ReadOperationCount;
	LARGE_INTEGER WriteOperationCount;
	LARGE_INTEGER OtherOperationCount;
	LARGE_INTEGER ReadTransferCount;
	LARGE_INTEGER WriteTransferCount;
	LARGE_INTEGER OtherTransferCount;
} REAL_SYSTEM_PROCESS_INFORMATION, *PREAL_SYSTEM_PROCESS_INFORMATION;


BOOL
SidToUserName(PSID pSid,
	LPWSTR szBuffer,
	DWORD BufferSize)
{
	WCHAR szDomainNameUnused[255];
	DWORD DomainNameLen;
	SID_NAME_USE Use;
	BOOL bRet = FALSE;

	DomainNameLen = sizeof(szDomainNameUnused) / sizeof(szDomainNameUnused[0]);

	if (pSid)
	{
		bRet = LookupAccountSidW(NULL,
			pSid,
			szBuffer,
			&BufferSize,
			szDomainNameUnused,
			&DomainNameLen,
			&Use);
	}

	return bRet;
}

BOOL
IsUserSid(PSID pSid)
{
	LSA_HANDLE PolicyHandle = NULL;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	PLSA_REFERENCED_DOMAIN_LIST ReferencedDomain = NULL;
	PLSA_TRANSLATED_NAME TranslatedName = NULL;
	NTSTATUS Status;
	BOOL bRet = FALSE;

	Status = LsaOpenPolicy(NULL,
		&ObjectAttributes,
		POLICY_LOOKUP_NAMES,
		&PolicyHandle);
	if (!NT_SUCCESS(Status))
	{
		SetLastError(LsaNtStatusToWinError(Status));
		return FALSE;
	}

	Status = LsaLookupSids(PolicyHandle,
		1,
		&pSid,
		&ReferencedDomain,
		&TranslatedName);
	LsaClose(PolicyHandle);

	if (NT_SUCCESS(Status))
	{
		SetLastError(ERROR_SUCCESS);

		if (TranslatedName->Use == SidTypeUser)
			bRet = TRUE;
	}
	else
	{
		SetLastError(LsaNtStatusToWinError(Status));
	}

	return bRet;
}

DWORD
GetUsernameFromSessionId(DWORD dwSessionId,
	LPWSTR lpUserName,
	DWORD dwNumChars)
{
	PREAL_SYSTEM_PROCESS_INFORMATION pSPI = NULL;
	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = 0;
	DWORD dwSizeNeeded;
	HANDLE hProcess;
	HANDLE hToken;
	PTOKEN_USER pTokenUser;
	PSID pSid = NULL;
	BOOL bFound;
	NTSTATUS Status;
	DWORD dwError;


	/* Let's start with 1000 bytes */
	dwSizeNeeded = 1000;
	do
	{
		/* Free any previous allocations */
		if (pBuffer)
		{
			HeapFree(GetProcessHeap(),
				0,
				pBuffer);
		}

		/* Get some memory to store our process info */
		pBuffer = (PBYTE)HeapAlloc(GetProcessHeap(),
			0,
			dwSizeNeeded);
		if (pBuffer)
		{
			dwBufferSize = dwSizeNeeded;

			/* Call into the kernel to get our list of processes */
			Status = NtQuerySystemInformation(SystemProcessInformation,
				pBuffer,
				dwBufferSize,
				&dwSizeNeeded);
		}
		else
		{
			/* Something went wrong */
			Status = STATUS_NO_MEMORY;
		}

		/* Keep looping until we have everything */
	} while (Status == STATUS_INFO_LENGTH_MISMATCH);


	/* Did we succeed? */
	if (NT_SUCCESS(Status))
	{
		/* Cast the buffer */
		pSPI = (PREAL_SYSTEM_PROCESS_INFORMATION)pBuffer;

		/* Loop through all the processes */
		bFound = FALSE;
		while (!bFound && pSPI)
		{
			/* Skip the system and idle processes */
			if (pSPI->UniqueProcessId != (HANDLE)0x00 &&
				pSPI->UniqueProcessId != (HANDLE)0x04)
			{
				/* Does this process's session id match ours? */
				if (pSPI->SessionId = dwSessionId)
				{
					/* It does, get a handle to it */
					hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | READ_CONTROL,
						FALSE,
						PtrToUlong(pSPI->UniqueProcessId));
					if (hProcess)
					{
						/* Get the token for this process */
						if (OpenProcessToken(hProcess,
							TOKEN_QUERY,
							&hToken))
						{
							/* Get the buffer size required to hold the token info */
							if (!GetTokenInformation(hToken,
								TokenUser,
								NULL,
								0,
								&dwSizeNeeded) &&
								GetLastError() == ERROR_INSUFFICIENT_BUFFER)
							{
								/* Get some memory to hold the info */
								pTokenUser = (PTOKEN_USER)HeapAlloc(GetProcessHeap(),
									0,
									dwSizeNeeded);
								if (pTokenUser)
								{
									/* Now get the info */
									if (GetTokenInformation(hToken,
										TokenUser,
										pTokenUser,
										dwSizeNeeded,
										&dwSizeNeeded))
									{
										if (IsUserSid(pTokenUser->User.Sid))
										{
											/* Get a copy of the SID */
											dwSizeNeeded = GetLengthSid(pTokenUser->User.Sid);
											pSid = HeapAlloc(GetProcessHeap(),
												0,
												dwSizeNeeded);
											if (pSid)
											{
												/* Copy the SID */
												bFound = CopySid(dwSizeNeeded, pSid, pTokenUser->User.Sid);
												if (!bFound)
												{
													/* Something went wrong */
													HeapFree(GetProcessHeap(),
														0,
														pSid);
													pSid = NULL;
												}
											}
										}
									}

									/* Free the token info */
									HeapFree(GetProcessHeap(),
										0,
										pTokenUser);
								}
							}

							/* Close the token handle */
							CloseHandle(hToken);
						}

						/* Close the process handle */
						CloseHandle(hProcess);
					}
				}
			}

			/* Move to the next process */
			pSPI = (PREAL_SYSTEM_PROCESS_INFORMATION)((LPBYTE)pSPI + pSPI->NextEntryOffset);
		}

		/* Did we find a user sid? */
		if (bFound)
		{
			/* Get the username for the sid */
			if (SidToUserName(pSid,
				lpUserName,
				dwNumChars))
			{
				dwError = ERROR_SUCCESS;
			}
			else
			{
				dwError = GetLastError();
			}
		}

		/* We're done with the sid now */
		HeapFree(GetProcessHeap(),
			0,
			pSid);
	}

	/* Free the process info */
	if (pBuffer)
	{
		HeapFree(GetProcessHeap(),
			0,
			pBuffer);
	}

	return dwError;
}