/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "globals.h"
#include "kull_m_process.h"
#include "kull_m_crypto_system.h"

typedef VOID  (WINAPI *FuncPtr_RtlInitUnicodeString)(OUT PUNICODE_STRING DestinationString, IN PCWSTR SourceString);

typedef BOOLEAN (WINAPI *FuncPtr_RtlEqualString)(IN const STRING *String1, IN const STRING *String2, IN BOOLEAN CaseInSensitive);
typedef BOOLEAN (WINAPI *FuncPtr_RtlEqualUnicodeString)(IN PCUNICODE_STRING String1, IN PCUNICODE_STRING String2, IN BOOLEAN CaseInSensitive);

typedef VOID (WINAPI *FuncPtr_RtlFreeUnicodeString)(IN PUNICODE_STRING UnicodeString);

typedef NTSTATUS (WINAPI *FuncPtr_RtlStringFromGUID)(IN LPCGUID Guid, PUNICODE_STRING UnicodeString);
typedef NTSTATUS (WINAPI *FuncPtr_RtlAppendUnicodeToString)(IN OUT PUNICODE_STRING  Destination, IN PCWSTR  Source);

typedef VOID (WINAPI *FuncPtr_RtlGetNtVersionNumbers)(LPDWORD pMajor, LPDWORD pMinor, LPDWORD pBuild);

typedef PPEB (WINAPI *FuncPtr_RtlGetCurrentPeb)();
typedef NTSTATUS (WINAPI *FuncPtr_RtlAdjustPrivilege)(IN ULONG Privilege, IN BOOL Enable, IN BOOL CurrentThread, OUT PULONG pPreviousState);

typedef NTSTATUS (WINAPI *FuncPtr_NtQuerySystemInformation)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT OPTIONAL PULONG ReturnLength);
typedef NTSTATUS (WINAPI *FuncPtr_NtQueryInformationProcess)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, OUT ULONG ProcessInformationLength, OUT OPTIONAL PULONG ReturnLength);

typedef VOID (WINAPI *FuncPtr_MD4Init)(PMD4_CTX pCtx);
typedef VOID (WINAPI *FuncPtr_MD4Update)(PMD4_CTX pCtx, LPCVOID data, DWORD cbData);
typedef VOID (WINAPI *FuncPtr_MD4Final)(PMD4_CTX pCtx);

typedef void (WINAPI* FuncPtr_MD5INIT)(MD5_CTX* context);
typedef void (WINAPI* FuncPtr_MD5UPDATE)(MD5_CTX* context, const unsigned char* input, unsigned int inlen);
typedef void (WINAPI* FuncPtr_MD5FINAL)(MD5_CTX* context);

extern FuncPtr_RtlInitUnicodeString Ptr_RtlInitUnicodeString;
extern FuncPtr_RtlEqualString Ptr_RtlEqualString;
extern FuncPtr_RtlEqualUnicodeString Ptr_RtlEqualUnicodeString;
extern FuncPtr_RtlFreeUnicodeString Ptr_RtlFreeUnicodeString;
extern FuncPtr_RtlStringFromGUID Ptr_RtlStringFromGUID;
extern FuncPtr_RtlAppendUnicodeToString Ptr_RtlAppendUnicodeToString;
extern FuncPtr_RtlGetNtVersionNumbers Ptr_RtlGetNtVersionNumbers;
extern FuncPtr_RtlGetCurrentPeb Ptr_RtlGetCurrentPeb;
extern FuncPtr_RtlAdjustPrivilege Ptr_RtlAdjustPrivilege;
extern FuncPtr_NtQuerySystemInformation Ptr_NtQuerySystemInformation;
extern FuncPtr_NtQueryInformationProcess Ptr_NtQueryInformationProcess;
extern FuncPtr_MD4Init Ptr_MD4Init;
extern FuncPtr_MD4Update Ptr_MD4Update;
extern FuncPtr_MD4Final Ptr_MD4Final;

BOOL kull_m_ntdll_init();
