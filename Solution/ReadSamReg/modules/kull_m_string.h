/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "globals.h"

typedef CONST char *PCSZ;
typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;
typedef PSTRING PCANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;
typedef CONST STRING* PCOEM_STRING;
typedef CONST UNICODE_STRING *PCUNICODE_STRING;

typedef struct _LSA_UNICODE_STRING64 {
	USHORT Length;
	USHORT MaximumLength;
#ifdef MIDL_PASS
	[size_is(MaximumLength / 2), length_is(Length / 2)]
#endif // MIDL_PASS
	DWORD64/*PWSTR*/  Buffer;
} LSA_UNICODE_STRING64, *PLSA_UNICODE_STRING64, UNICODE_STRING64;

#define DECLARE_UNICODE_STRING(_var, _string) \
const WCHAR _var ## _buffer[] = _string; \
UNICODE_STRING _var = { sizeof(_string) - sizeof(WCHAR), sizeof(_string), (PWCH) _var ## _buffer }

#define OUTPUT_LSASTRING64_BUFFER(str)	((str) ? ((LPWSTR)((str)->Buffer)) : (NULL)) 
#define OUTPUT_LSASTRING64_LENGTH(str)	((str) ? (((str)->Length) / (sizeof(WCHAR))) : (0))

extern VOID WINAPI RtlInitString(OUT PSTRING DestinationString, IN PCSZ SourceString);
extern VOID WINAPI RtlInitUnicodeString(OUT PUNICODE_STRING DestinationString, IN PCWSTR SourceString);

extern NTSTATUS WINAPI RtlAnsiStringToUnicodeString(OUT PUNICODE_STRING DestinationString, IN PCANSI_STRING SourceString, IN BOOLEAN AllocateDestinationString);
extern NTSTATUS WINAPI RtlUnicodeStringToAnsiString(OUT PANSI_STRING DestinationString, IN PCUNICODE_STRING SourceString, IN BOOLEAN AllocateDestinationString);

extern VOID WINAPI RtlUpperString(OUT PSTRING DestinationString, IN const STRING *SourceString);
extern NTSTATUS WINAPI RtlUpcaseUnicodeString(IN OUT PUNICODE_STRING DestinationString, IN PCUNICODE_STRING SourceString, IN BOOLEAN AllocateDestinationString);
extern NTSTATUS WINAPI RtlDowncaseUnicodeString(PUNICODE_STRING DestinationString, IN PCUNICODE_STRING SourceString, IN BOOLEAN AllocateDestinationString);
extern WCHAR WINAPI RtlUpcaseUnicodeChar(IN WCHAR SourceCharacter);

extern BOOLEAN WINAPI RtlEqualString(IN const STRING *String1, IN const STRING *String2, IN BOOLEAN CaseInSensitive);
extern BOOLEAN WINAPI RtlEqualUnicodeString(IN PCUNICODE_STRING String1, IN PCUNICODE_STRING String2, IN BOOLEAN CaseInSensitive);

extern LONG WINAPI RtlCompareUnicodeString(IN PCUNICODE_STRING String1, IN PCUNICODE_STRING String2, IN BOOLEAN CaseInSensitive);
extern LONG WINAPI RtlCompareString(IN const STRING *String1, IN const STRING *String2, IN BOOLEAN CaseInSensitive);

extern VOID WINAPI RtlFreeAnsiString(IN PANSI_STRING AnsiString);
extern VOID WINAPI RtlFreeUnicodeString(IN PUNICODE_STRING UnicodeString);

extern NTSTATUS WINAPI RtlStringFromGUID(IN LPCGUID Guid, PUNICODE_STRING UnicodeString);
extern NTSTATUS WINAPI RtlGUIDFromString(IN PCUNICODE_STRING GuidString, OUT GUID *Guid);
extern NTSTATUS NTAPI RtlValidateUnicodeString(IN ULONG Flags, IN PCUNICODE_STRING UnicodeString);

extern NTSTATUS WINAPI RtlAppendUnicodeStringToString(IN OUT PUNICODE_STRING Destination, IN PCUNICODE_STRING Source);
extern NTSTATUS RtlAppendUnicodeToString(IN OUT PUNICODE_STRING  Destination, IN PCWSTR  Source);

extern VOID NTAPI RtlRunDecodeUnicodeString(IN BYTE Hash, IN OUT PUNICODE_STRING String);
extern VOID NTAPI RtlRunEncodeUnicodeString(IN OUT PBYTE Hash, IN OUT PUNICODE_STRING String);

void kull_m_string_MakeRelativeOrAbsoluteString(PVOID BaseAddress, PLSA_UNICODE_STRING String, BOOL relative);
void kull_m_string_MakeRelativeOrAbsoluteString64(PVOID BaseAddress, PLSA_UNICODE_STRING64 String, BOOL relative);
BOOL kull_m_string_copyUnicodeStringBuffer(PUNICODE_STRING pSource, PUNICODE_STRING pDestination);
BOOL kull_m_string_copyUnicodeStringBuffer64(PLSA_UNICODE_STRING64 pSource, PUNICODE_STRING pDestination);
void kull_m_string_freeUnicodeStringBuffer(PUNICODE_STRING pString);
void kull_m_string_clearUnicodeString(PUNICODE_STRING pString);
BOOL kull_m_string_suspectUnicodeString(IN PUNICODE_STRING pUnicodeString);
BOOL kull_m_string_suspectUnicodeString64(IN PLSA_UNICODE_STRING64 pUnicodeString);
void kull_m_string_printSuspectUnicodeString(PVOID data, DWORD size);
void kull_m_string_LsaUnicode64ToLsaUnicode(PLSA_UNICODE_STRING64 pSource, PUNICODE_STRING pDst);

wchar_t * kull_m_string_qad_ansi_to_unicode(const char * ansi);
wchar_t * kull_m_string_qad_ansi_c_to_unicode(const char * ansi, SIZE_T szStr);
char * kull_m_string_unicode_to_ansi(const wchar_t * unicode);
BOOL kull_m_string_stringToHex(IN LPCWCHAR string, IN LPBYTE hex, IN DWORD size);
BOOL kull_m_string_stringToHexBuffer(IN LPCWCHAR string, IN LPBYTE *hex, IN DWORD *size);

void kull_m_string_wprintf_hex(LPCVOID lpData, DWORD cbData, DWORD flags);
void kull_m_string_displayFileTime(IN PFILETIME pFileTime);
void kull_m_string_displayLocalFileTime(IN PFILETIME pFileTime);
BOOL kull_m_string_FileTimeToString(IN PFILETIME pFileTime, OUT WCHAR string[14 + 1]);
void kull_m_string_displayGUID(IN LPCGUID pGuid);
void kull_m_string_displaySID(IN PSID pSid);
PWSTR kull_m_string_getRandomGUID();
void kull_m_string_ptr_replace(PVOID ptr, DWORD64 size);

BOOL kull_m_string_args_byName(const int argc, const wchar_t * argv[], const wchar_t * name, const wchar_t ** theArgs, const wchar_t * defaultValue);
BOOL kull_m_string_args_bool_byName(int argc, wchar_t * argv[], LPCWSTR name, PBOOL value);
BOOL kull_m_string_copy(LPWSTR *dst, LPCWSTR src);
BOOL kull_m_string_copyA(LPSTR *dst, LPCSTR src);
BOOL kull_m_string_quickxml_simplefind(LPCWSTR xml, LPCWSTR node, LPWSTR *dst);
#ifndef MIMIKATZ_W2000_SUPPORT
BOOL kull_m_string_quick_base64_to_Binary(PCWSTR base64, PBYTE *data, DWORD *szData);
#endif