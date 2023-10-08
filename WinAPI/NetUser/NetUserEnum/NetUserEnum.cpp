#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <assert.h>
#include <windows.h> 
#include <lm.h>
#include <iostream>
using namespace std;

void enmuUser(int argc, wchar_t *argv[])
{
	LPUSER_INFO_0 pBuf = NULL;
	LPUSER_INFO_0 pTmpBuf;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	DWORD dwResumeHandle = 0;
	DWORD i;
	DWORD dwTotalCount = 0;
	NET_API_STATUS nStatus;
	LPTSTR pszServerName = NULL;

	if (argc > 2)
	{
		fwprintf(stderr, L"Usage: %s [\\\\ServerName]\n", argv[0]);
		exit(1);
	}
	// The server is not the default local computer.
	//
	if (argc == 2)
		pszServerName = (LPTSTR)argv[1];
	wprintf(L"\nUser account on %s: \n", pszServerName);
	//
	// Call the NetUserEnum function, specifying level 0; 
	//   enumerate global user account types only.
	//
	do // begin do
	{
		nStatus = NetUserEnum((LPCWSTR)pszServerName,
			dwLevel,
			FILTER_NORMAL_ACCOUNT, // global users
			(LPBYTE*)&pBuf,
			dwPrefMaxLen,
			&dwEntriesRead,
			&dwTotalEntries,
			&dwResumeHandle);
		//
		// If the call succeeds,
		//
		if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
		{
			if ((pTmpBuf = pBuf) != NULL)
			{
				//
				// Loop through the entries.
				//
				for (i = 0; (i < dwEntriesRead); i++)
				{
					assert(pTmpBuf != NULL);

					if (pTmpBuf == NULL)
					{
						fprintf(stderr, "An access violation has occurred\n");
						break;
					}
					//
					//  Print the name of the user account.
					//
					wprintf(L"\t-- %s\n", pTmpBuf->usri0_name);

					pTmpBuf++;
					dwTotalCount++;
				}
			}
		}
		//
		// Otherwise, print the system error.
		//
		else
			fprintf(stderr, "A system error has occurred: %d\n", nStatus);
		//
		// Free the allocated buffer.
		//
		if (pBuf != NULL)
		{
			NetApiBufferFree(pBuf);
			pBuf = NULL;
		}
	}
	// Continue to call NetUserEnum while 
	//  there are more entries. 
	// 
	while (nStatus == ERROR_MORE_DATA); // end do
	//
	// Check again for allocated memory.
	//
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);
	//
	// Print the final count of users enumerated.
	//
	fprintf(stderr, "\nTotal of %d entries enumerated\n", dwTotalCount);

}

void disableUser(int argc, wchar_t *argv[])
{
	LPUSER_INFO_1 pBuf = NULL;
	LPUSER_INFO_1 pTmpBuf;
	DWORD dwLevel = 1;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	DWORD dwResumeHandle = 0;
	DWORD i;
	DWORD dwTotalCount = 0;
	NET_API_STATUS nStatus;
	LPTSTR pszServerName = NULL;
	LPUSER_INFO_1 pBuf1 = NULL;

	if (argc > 2)
	{
		fwprintf(stderr, L"Usage: %s [\\\\ServerName]\n", argv[0]);
		exit(1);
	}
	// The server is not the default local computer.
	//
	if (argc == 2)
		pszServerName = (LPTSTR)argv[1];
	wprintf(L"\nUser account on %s: \n", pszServerName);
	//
	// Call the NetUserEnum function, specifying level 0; 
	//   enumerate global user account types only.
	//
	do // begin do
	{
		nStatus = NetUserEnum((LPCWSTR)pszServerName,
			dwLevel,
			FILTER_NORMAL_ACCOUNT, // global users
			(LPBYTE*)&pBuf,
			dwPrefMaxLen,
			&dwEntriesRead,
			&dwTotalEntries,
			&dwResumeHandle);
		//
		// If the call succeeds,
		//

		nStatus = NetUserGetInfo(NULL, pBuf->usri1_name, 1, reinterpret_cast<LPBYTE*>(&pBuf1));

		if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
		{
			if ((pTmpBuf = pBuf) != NULL)
			{
				//
				// Loop through the entries.
				//
				for (i = 0; (i < dwEntriesRead); i++)
				{
					assert(pTmpBuf != NULL);

					if (pTmpBuf == NULL)
					{
						fprintf(stderr, "An access violation has occurred\n");
						break;
					}
					//
					//  Print the name of the user account.
					//
					wprintf(L"\t-- %s\n", pTmpBuf->usri1_name);

					/************************************************************************/
					/*                                                                      */
					/************************************************************************/
					if (0 == lstrcmp(pTmpBuf->usri1_name, L"Guest"))
					{
						USER_INFO_1008 ui;
						ui.usri1008_flags = (*pBuf1).usri1_flags | UF_ACCOUNTDISABLE;

						dwLevel = 1008;

						nStatus = NetUserSetInfo((LPCWSTR)pszServerName,
							pTmpBuf->usri1_name,
							dwLevel,
							(LPBYTE)&ui,
							NULL);
						//
						// Display the result of the call.
						//
						if (nStatus == NERR_Success)
							fwprintf(stderr, L"User account %s has been disabled\n", pTmpBuf->usri1_name);
						else
							fprintf(stderr, "A system error has occurred: %d\n", nStatus);
					}
					else
					{
						printf("Î´ÕÒµ½Guest\n");
					}
					/************************************************************************/
					/*                                                                      */
					/************************************************************************/

					pTmpBuf++;
					dwTotalCount++;
				}
			}
		}
		//
		// Otherwise, print the system error.
		//
		else
			fprintf(stderr, "A system error has occurred: %d\n", nStatus);
		//
		// Free the allocated buffer.
		//
		if (pBuf != NULL)
		{
			NetApiBufferFree(pBuf);
			pBuf = NULL;
		}
	}
	// Continue to call NetUserEnum while 
	//  there are more entries. 
	// 
	while (nStatus == ERROR_MORE_DATA); // end do
	//
	// Check again for allocated memory.
	//
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);
	//
	// Print the final count of users enumerated.
	//
	fprintf(stderr, "\nTotal of %d entries enumerated\n", dwTotalCount);
}
int main1(int argc, wchar_t *argv[])
{
	DWORD dw;
	cin >> dw;
	if (dw == 1 )
	{
		enmuUser(argc, argv);
	}
	else
	{
		disableUser(argc, argv);
	}
	
	return 0;
}
