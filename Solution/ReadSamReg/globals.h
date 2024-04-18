#pragma once

#include <ntstatus.h>
#define WIN32_NO_STATUS
#define SECURITY_WIN32
#define CINTERFACE
#define COBJMACROS
#include <windows.h>
#include <NTSecAPI.h>
#include <sspi.h>
#include <sddl.h>
#include <wincred.h>
#include <ntsecapi.h>
#include <ntsecpkg.h>
#include <stdio.h>
#include <wchar.h>

#include "../modules/kull_m_output.h"
//#define KERBEROS_TOOLS
//#define SERVICE_INCONTROL
//#define LSASS_DECRYPT
#define NET_MODULE
#define SQLITE3_OMIT
#ifdef _M_X64
#define MIMIKATZ_ARCH L"x64"
#else ifdef _M_IX86
#define MIMIKATZ_ARCH L"x86"
#endif

#define LoadLibrary_api LoadLibrary

#define MIMIKATZ				L"mimikatz"
#define MIMIKATZ_VERSION		L"2.1.1"
#define MIMIKATZ_CODENAME		L"A La Vie, A L\'Amour"
#define MIMIKATZ_FULL			MIMIKATZ L" " MIMIKATZ_VERSION L" (" MIMIKATZ_ARCH L") built on " TEXT(__DATE__) L" " TEXT(__TIME__)
#define MIMIKATZ_SECOND			L"\"" MIMIKATZ_CODENAME L"\""
#define MIMIKATZ_SPECIAL		L"                                "
#define MIMIKATZ_DEFAULT_LOG	MIMIKATZ L".log"
#define MIMIKATZ_DRIVER			L"mimidrv"
#define MIMIKATZ_KERBEROS_EXT	L"kirbi"
#define MIMIKATZ_SERVICE		MIMIKATZ L"svc"

#ifdef _WINDLL
#define MIMIKATZ_AUTO_COMMAND_START		0
#define MIMIKATZ_AUTO_COMMAND_STRING	L"powershell"
#else
#define MIMIKATZ_AUTO_COMMAND_START		1
#define MIMIKATZ_AUTO_COMMAND_STRING	L"commandline"
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

#ifndef PRINT_ERROR
#define PRINT_ERROR(...) (kprintf(L"ERROR " TEXT(__FUNCTION__) L" ; " __VA_ARGS__))
#endif

#ifndef PRINT_ERROR_AUTO
#define PRINT_ERROR_AUTO(func) (kprintf(L"ERROR " TEXT(__FUNCTION__) L" ; " func L" (0x%08x)\n", GetLastError()))
#endif

#ifndef W00T
#define W00T(...) (kprintf(TEXT(__FUNCTION__) L" w00t! ; " __VA_ARGS__))
#endif

DWORD MIMIKATZ_NT_MAJOR_VERSION, MIMIKATZ_NT_MINOR_VERSION, MIMIKATZ_NT_BUILD_NUMBER;

#ifndef MS_ENH_RSA_AES_PROV_XP
#define MS_ENH_RSA_AES_PROV_XP	L"Microsoft Enhanced RSA and AES Cryptographic Provider (Prototype)"
#endif

#ifndef SCARD_PROVIDER_CARD_MODULE
#define SCARD_PROVIDER_CARD_MODULE 0x80000001
#endif

#define RtlEqualGuid(L1, L2) (RtlEqualMemory(L1, L2, sizeof(GUID)))

#define SIZE_ALIGN(size, alignment)	(size + ((size % alignment) ? (alignment - (size % alignment)) : 0))
#define KIWI_NEVERTIME(filetime)	(*(PLONGLONG) filetime = MAXLONGLONG)

typedef PVOID	SAMPR_HANDLE;


#ifndef _NTDEF_
typedef LSA_UNICODE_STRING UNICODE_STRING, * PUNICODE_STRING;
typedef LSA_STRING STRING, * PSTRING;
#endif

#if !defined(NT_SUCCESS)
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

#define	SYSKEY_LENGTH	16
#define	SAM_KEY_DATA_SALT_LENGTH	16
#define	SAM_KEY_DATA_KEY_LENGTH		16

#define	MD4_DIGEST_LENGTH	16
#define	MD5_DIGEST_LENGTH	16
#define SHA_DIGEST_LENGTH	20

#define	DES_KEY_LENGTH		7
#define DES_BLOCK_LENGTH	8
#define AES_128_KEY_LENGTH	16
#define AES_256_KEY_LENGTH	32

#if !defined(IPSEC_FLAG_CHECK)
#define IPSEC_FLAG_CHECK 0xf42a19b6
#endif

#define LM_NTLM_HASH_LENGTH	16

typedef CONST UNICODE_STRING* PCUNICODE_STRING;

typedef struct _SAM_ENTRY {
	DWORD offset;
	DWORD lenght;
	DWORD unk;
} SAM_ENTRY, * PSAM_SENTRY;

typedef struct _KIWI_BACKUP_KEY {
	DWORD version;
	DWORD keyLen;
	DWORD certLen;
	BYTE data[ANYSIZE_ARRAY];
} KIWI_BACKUP_KEY, * PKIWI_BACKUP_KEY;

typedef struct _NTDS_LSA_AUTH_INFORMATION {
	LARGE_INTEGER LastUpdateTime;
	ULONG AuthType;
	ULONG AuthInfoLength;
	UCHAR AuthInfo[ANYSIZE_ARRAY]; //
} NTDS_LSA_AUTH_INFORMATION, * PNTDS_LSA_AUTH_INFORMATION;

typedef struct _NTDS_LSA_AUTH_INFORMATIONS {
	DWORD count; // or version ?
	DWORD offsetToAuthenticationInformation;	// PLSA_AUTH_INFORMATION
	DWORD offsetToPreviousAuthenticationInformation;	// PLSA_AUTH_INFORMATION
	// ...
} NTDS_LSA_AUTH_INFORMATIONS, * PNTDS_LSA_AUTH_INFORMATIONS;

typedef wchar_t* LOGONSRV_HANDLE;
typedef struct _NETLOGON_CREDENTIAL {
	CHAR data[8];
} NETLOGON_CREDENTIAL, * PNETLOGON_CREDENTIAL;

typedef  enum _NETLOGON_SECURE_CHANNEL_TYPE {
	NullSecureChannel = 0,
	MsvApSecureChannel = 1,
	WorkstationSecureChannel = 2,
	TrustedDnsDomainSecureChannel = 3,
	TrustedDomainSecureChannel = 4,
	UasServerSecureChannel = 5,
	ServerSecureChannel = 6,
	CdcServerSecureChannel = 7
} NETLOGON_SECURE_CHANNEL_TYPE;

typedef struct _NETLOGON_AUTHENTICATOR {
	NETLOGON_CREDENTIAL Credential;
	DWORD Timestamp;
} NETLOGON_AUTHENTICATOR, * PNETLOGON_AUTHENTICATOR;

typedef struct _NL_TRUST_PASSWORD {
	WCHAR Buffer[256];
	ULONG Length;
} NL_TRUST_PASSWORD, * PNL_TRUST_PASSWORD;

typedef struct _CYPHER_BLOCK {
	CHAR data[8];
} CYPHER_BLOCK, * PCYPHER_BLOCK;

#define MIMIKATZ				L"mimikatz"
#define LSA_CREDENTIAL_KEY_PACKAGE_NAME			L"LSACREDKEY"


typedef struct _CRYPT_BUFFER {
	DWORD Length;
	DWORD MaximumLength;
	PVOID Buffer;
} CRYPT_BUFFER, * PCRYPT_BUFFER, DATA_KEY, * PDATA_KEY, CLEAR_DATA, * PCLEAR_DATA, CYPHER_DATA, * PCYPHER_DATA;

#pragma pack(push, 1) 
typedef struct _USER_PROPERTY {
	USHORT NameLength;
	USHORT ValueLength;
	USHORT Reserved;
	wchar_t PropertyName[ANYSIZE_ARRAY];
	// PropertyValue in HEX !
} USER_PROPERTY, * PUSER_PROPERTY;

typedef struct _USER_PROPERTIES {
	DWORD Reserved1;
	DWORD Length;
	USHORT Reserved2;
	USHORT Reserved3;
	BYTE Reserved4[96];
	wchar_t PropertySignature;
	USHORT PropertyCount;
	USER_PROPERTY UserProperties[ANYSIZE_ARRAY];
} USER_PROPERTIES, * PUSER_PROPERTIES;
#pragma pack(pop)


typedef struct _PAC_CREDENTIAL_DATA {
	ULONG CredentialCount;
	SECPKG_SUPPLEMENTAL_CRED Credentials[ANYSIZE_ARRAY];
} PAC_CREDENTIAL_DATA, * PPAC_CREDENTIAL_DATA;

typedef struct _NTLM_SUPPLEMENTAL_CREDENTIAL {
	ULONG Version;
	ULONG Flags;
	UCHAR LmPassword[LM_NTLM_HASH_LENGTH];
	UCHAR NtPassword[LM_NTLM_HASH_LENGTH];
} NTLM_SUPPLEMENTAL_CREDENTIAL, * PNTLM_SUPPLEMENTAL_CREDENTIAL;

typedef enum _KIWI_CREDENTIAL_KEY_TYPE {
	CREDENTIALS_KEY_TYPE_NTLM = 1,
	CREDENTIALS_KEY_TYPE_SHA1 = 2,
	CREDENTIALS_KEY_TYPE_ROOTKEY = 3,
	CREDENTIALS_KEY_TYPE_DPAPI_PROTECTION = 4,
} KIWI_CREDENTIAL_KEY_TYPE;

typedef struct _KIWI_CREDENTIAL_KEY {
	DWORD unkEnum; // version ?
	KIWI_CREDENTIAL_KEY_TYPE type;
	WORD iterations;
	WORD cbData;
	BYTE* pbData;
} KIWI_CREDENTIAL_KEY, * PKIWI_CREDENTIAL_KEY;

typedef struct _KIWI_CREDENTIAL_KEYS {
	DWORD count;
	KIWI_CREDENTIAL_KEY keys[ANYSIZE_ARRAY];
} KIWI_CREDENTIAL_KEYS, * PKIWI_CREDENTIAL_KEYS;

typedef struct _REMOTE_EXT {
	PCWCHAR	Module;
	PCHAR	Function;
	PVOID	ToReplace;
	PVOID	Pointer;
} REMOTE_EXT, * PREMOTE_EXT;

typedef struct _MULTIPLE_REMOTE_EXT {
	DWORD count;
	PREMOTE_EXT extensions;
} MULTIPLE_REMOTE_EXT, * PMULTIPLE_REMOTE_EXT;

#define AES_256_KEY_SIZE	(256/8)
#define AES_128_KEY_SIZE	(128/8)
#define AES_BLOCK_SIZE		16

#define KULL_M_WIN_BUILD_XP		2600
#define KULL_M_WIN_BUILD_2K3	3790
#define KULL_M_WIN_BUILD_VISTA	6000
#define KULL_M_WIN_BUILD_7		7600
#define KULL_M_WIN_BUILD_8		9200
#define KULL_M_WIN_BUILD_BLUE	9600
#define KULL_M_WIN_BUILD_10_1507	10240
#define KULL_M_WIN_BUILD_10_1511	10586
#define KULL_M_WIN_BUILD_10_1607	14393
#define KULL_M_WIN_BUILD_10_1703	15063
#define KULL_M_WIN_BUILD_10_1709	16299
#define KULL_M_WIN_BUILD_10_1803	17134
#define KULL_M_WIN_BUILD_10_1809	17763
#define KULL_M_WIN_BUILD_10_1903	18362
#define KULL_M_WIN_BUILD_10_1909	18363
#define KULL_M_WIN_BUILD_10_2004	19041
#define KULL_M_WIN_BUILD_10_20H2	19042
#define KULL_M_WIN_BUILD_10_21H2	19044
#define KULL_M_WIN_BUILD_2022		20348
#define KULL_M_WIN_BUILD_11_22H2	22621

#define KULL_M_WIN_MIN_BUILD_XP		2500
#define KULL_M_WIN_MIN_BUILD_2K3	3000
#define KULL_M_WIN_MIN_BUILD_VISTA	5000
#define KULL_M_WIN_MIN_BUILD_7		7000
#define KULL_M_WIN_MIN_BUILD_8		8000
#define KULL_M_WIN_MIN_BUILD_BLUE	9400
#define KULL_M_WIN_MIN_BUILD_10		9800
#define KULL_M_WIN_MIN_BUILD_11		22000

#define USER_CHANGE_PASSWORD			0x00000040
#define USER_FORCE_PASSWORD_CHANGE		0x00000080

#define SAM_SERVER_CONNECT				0x00000001
#define SAM_SERVER_SHUTDOWN				0x00000002
#define SAM_SERVER_INITIALIZE			0x00000004
#define SAM_SERVER_CREATE_DOMAIN		0x00000008
#define SAM_SERVER_ENUMERATE_DOMAINS	0x00000010
#define SAM_SERVER_LOOKUP_DOMAIN		0x00000020
#define SAM_SERVER_ALL_ACCESS			0x000f003f
#define SAM_SERVER_READ					0x00020010
#define SAM_SERVER_WRITE				0x0002000e
#define SAM_SERVER_EXECUTE				0x00020021

#define DOMAIN_READ_PASSWORD_PARAMETERS	0x00000001
#define DOMAIN_WRITE_PASSWORD_PARAMS	0x00000002
#define DOMAIN_READ_OTHER_PARAMETERS	0x00000004
#define DOMAIN_WRITE_OTHER_PARAMETERS	0x00000008
#define DOMAIN_CREATE_USER				0x00000010
#define DOMAIN_CREATE_GROUP				0x00000020
#define DOMAIN_CREATE_ALIAS				0x00000040
#define DOMAIN_GET_ALIAS_MEMBERSHIP		0x00000080
#define DOMAIN_LIST_ACCOUNTS			0x00000100
#define DOMAIN_LOOKUP					0x00000200
#define DOMAIN_ADMINISTER_SERVER		0x00000400
#define DOMAIN_ALL_ACCESS				0x000f07ff
#define DOMAIN_READ						0x00020084
#define DOMAIN_WRITE					0x0002047a
#define DOMAIN_EXECUTE					0x00020301

#define RPC_EXCEPTION (RpcExceptionCode() != STATUS_ACCESS_VIOLATION) && \
	(RpcExceptionCode() != STATUS_DATATYPE_MISALIGNMENT) && \
	(RpcExceptionCode() != STATUS_PRIVILEGED_INSTRUCTION) && \
	(RpcExceptionCode() != STATUS_ILLEGAL_INSTRUCTION) && \
	(RpcExceptionCode() != STATUS_BREAKPOINT) && \
	(RpcExceptionCode() != STATUS_STACK_OVERFLOW) && \
	(RpcExceptionCode() != STATUS_IN_PAGE_ERROR) && \
	(RpcExceptionCode() != STATUS_ASSERTION_FAILURE) && \
	(RpcExceptionCode() != STATUS_STACK_BUFFER_OVERRUN) && \
	(RpcExceptionCode() != STATUS_GUARD_PAGE_VIOLATION)

const SEC_WINNT_AUTH_IDENTITY KULL_M_RPC_NULLSESSION;
#define KULL_M_RPC_AUTH_IDENTITY_HANDLE_NULLSESSION ((RPC_AUTH_IDENTITY_HANDLE) &KULL_M_RPC_NULLSESSION)

typedef struct _REMOTE_LIB_OUTPUT_DATA {
	PVOID		outputVoid;
	DWORD		outputDword;
	NTSTATUS	outputStatus;
	DWORD		outputSize;
	PVOID		outputData;
} REMOTE_LIB_OUTPUT_DATA, * PREMOTE_LIB_OUTPUT_DATA;

typedef struct _REMOTE_LIB_INPUT_DATA {
	PVOID		inputVoid;
	DWORD		inputDword;
	DWORD		inputSize;
	BYTE		inputData[ANYSIZE_ARRAY];
} REMOTE_LIB_INPUT_DATA, * PREMOTE_LIB_INPUT_DATA;

typedef struct _REMOTE_LIB_DATA {
	REMOTE_LIB_OUTPUT_DATA	output;
	REMOTE_LIB_INPUT_DATA	input;
} REMOTE_LIB_DATA, * PREMOTE_LIB_DATA;


#define DECLARE_CONST_UNICODE_STRING(_var, _string) \
const WCHAR _var ## _buffer[] = _string; \
const UNICODE_STRING _var = { sizeof(_string) - sizeof(WCHAR), sizeof(_string), (PWCH) _var ## _buffer }

#define DECLARE_UNICODE_STRING(_var, _string) \
const WCHAR _var ## _buffer[] = _string; \
UNICODE_STRING _var = { sizeof(_string) - sizeof(WCHAR), sizeof(_string), (PWCH) _var ## _buffer }

typedef struct _SAM_USER {
	TCHAR szUsername[120];
	TCHAR szHash[120];
}_SAM_USER;