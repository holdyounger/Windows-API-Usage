#include "Ntsecapi.h"

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS  ((NTSTATUS)0x00000000L)
#endif

bool InitLsaString(
	PLSA_UNICODE_STRING pLsaString,
	LPCWSTR pwszString
)
{
	DWORD dwLen = 0;

	if (NULL == pLsaString)
		return FALSE;

	if (NULL != pwszString)
	{
		dwLen = wcslen(pwszString);
		if (dwLen > 0x7ffe)   // String is too large
			return FALSE;
	}

	// Store the string.
	pLsaString->Buffer = (WCHAR *)pwszString;
	pLsaString->Length = (USHORT)dwLen * sizeof(WCHAR);
	pLsaString->MaximumLength = (USHORT)(dwLen + 1) * sizeof(WCHAR);

	return TRUE;
}

NTSTATUS
OpenPolicy(
	LPWSTR ServerName,
	DWORD DesiredAccess,
	PLSA_HANDLE PolicyHandle
)
{
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	LSA_UNICODE_STRING ServerString;
	PLSA_UNICODE_STRING Server = NULL;

	// 
	// Always initialize the object attributes to all zeroes.
	// 
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	if (ServerName != NULL) {
		// 
		// Make a LSA_UNICODE_STRING out of the LPWSTR passed in
		// 
		InitLsaString(&ServerString, ServerName);
		Server = &ServerString;
	}

	// 
	// Attempt to open the policy.
	// 
	return LsaOpenPolicy(
		Server,
		&ObjectAttributes,
		DesiredAccess,
		PolicyHandle
	);
}

void GetPrivileges()
{
	//获取权限

	//LsaEnumerateAccountRights 


	PLSA_UNICODE_STRING pLucPrivilege = NULL;
	NTSTATUS ntsResult;
	LSA_HANDLE PolicyHandle;
	PSID AccountSID;

	ULONG CountOfRights = 0;

	//sid

	BOOL bRes = FALSE;

	TCHAR  userName[255] = L"administrators";
	char sid[255] = "";
	LPTSTR MySid = NULL;
	//DWORD nameSize = sizeof(userName);
	// GetUserName((LPWSTR)userName, &nameSize);


	char userSID[255] = "";
	char userDomain[255] = "";
	DWORD sidSize = sizeof(userSID);
	DWORD domainSize = sizeof(userDomain);


	SID_NAME_USE snu;
	LookupAccountName(NULL,
		(LPWSTR)userName,
		(PSID)userSID,
		&sidSize,
		(LPWSTR)userDomain,
		&domainSize,
		&snu);



	ConvertSidToStringSid((PSID)userSID, &MySid);
	//Sid = MySid;

	LocalFree(MySid);

	//AfxMessageBox(Sid);

	// 
	// Open the policy on the target machine.
	// 
	if ((ntsResult = OpenPolicy(
		NULL,      // target machine
		POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES,
		&PolicyHandle       // resultant policy handle
	)) != STATUS_SUCCESS)
	{
		//DisplayNtStatus("OpenPolicy", Status);
		CString strError;
		strError.Format(L"OpenPolicy fail - %lu \n", LsaNtStatusToWinError(ntsResult));
		AfxMessageBox(strError);
		return;//RTN_ERROR;
	}


	//// Create an LSA_UNICODE_STRING for the privilege names.
	//if (!InitLsaString(&lucPrivilege, L"SeSecurityPrivilege"))
	//{
	// wprintf(L"Failed InitLsaString\n");
	// AfxMessageBox(L"Failed InitLsaString SeSecurityPrivilege\n");
	// return;
	//}

	ntsResult = LsaEnumerateAccountRights(
		PolicyHandle,  // An open policy handle.
		(PSID)userSID,    // The target SID.
		&pLucPrivilege, // The privileges.
		&CountOfRights              // Number of privileges.
	);

	if (ntsResult == STATUS_SUCCESS)
	{
		wprintf(L"LsaEnumerateAccountRights added.\n");
		CString strTemp;
		strTemp.Format(_T("user=%s, AccountRights count=%d"), userName, CountOfRights);
		AfxMessageBox(strTemp);

		for (ULONG i = 0; i < CountOfRights; i++)
		{
			AfxMessageBox(pLucPrivilege[i].Buffer);
		}
	}
	else
	{
		wprintf(L"Privilege was not added - %lu \n", LsaNtStatusToWinError(ntsResult));
		CString strError;
		strError.Format(L"Privilege was not added - %lu , ntsResult=%x\n", LsaNtStatusToWinError(ntsResult), ntsResult);
		AfxMessageBox(strError);
	}


	if (pLucPrivilege)
	{
		LsaFreeMemory(pLucPrivilege);
	}
}