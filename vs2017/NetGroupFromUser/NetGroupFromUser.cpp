#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <assert.h>
#include <windows.h> 
#include <lm.h>

int wmain(int argc, wchar_t *argv[])
{
	argc = 3;
	LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
	DWORD dwLevel = 0;
	DWORD dwFlags = LG_INCLUDE_INDIRECT;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	NET_API_STATUS nStatus;

	if (argc != 3)
	{
		fwprintf(stderr, L"Usage: %s \\\\ServerName UserName\n", argv[0]);
		exit(1);
	}
	//
	// Call the NetUserGetLocalGroups function 
	//  specifying information level 0.
	//
	//  The LG_INCLUDE_INDIRECT flag specifies that the 
	//   function should also return the names of the local 
	//   groups in which the user is indirectly a member.
	//
	nStatus = NetUserGetLocalGroups(NULL,
		argv[1],
		dwLevel,
		dwFlags,
		(LPBYTE *)&pBuf,
		dwPrefMaxLen,
		&dwEntriesRead,
		&dwTotalEntries);
	//
	// If the call succeeds,
	//
	if (nStatus == NERR_Success)
	{
		LPLOCALGROUP_USERS_INFO_0 pTmpBuf;
		DWORD i;
		DWORD dwTotalCount = 0;

		if ((pTmpBuf = pBuf) != NULL)
		{
			fprintf(stderr, "\nLocal group(s):\n");
			//
			// Loop through the entries and 
			//  print the names of the local groups 
			//  to which the user belongs. 
			//
			for (i = 0; i < dwEntriesRead; i++)
			{
				assert(pTmpBuf != NULL);

				if (pTmpBuf == NULL)
				{
					fprintf(stderr, "An access violation has occurred\n");
					break;
				}

				wprintf(L"\t-- %s\n", pTmpBuf->lgrui0_name);

				pTmpBuf++;
				dwTotalCount++;
			}
		}
		//
		// If all available entries were
		//  not enumerated, print the number actually 
		//  enumerated and the total number available.
		//
		if (dwEntriesRead < dwTotalEntries)
			fprintf(stderr, "\nTotal entries: %d", dwTotalEntries);
		//
		// Otherwise, just print the total.
		//
		printf("\nEntries enumerated: %d\n", dwTotalCount);
	}
	else
		fprintf(stderr, "A system error has occurred: %d\n", nStatus);
	//
	// Free the allocated memory.
	//
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);

	return 0;
}