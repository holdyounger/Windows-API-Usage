#include "inc\WinReg.h"
#include "inc\winos.h"
#include "inc\winexception.h"
#include <windows.h>
#include <memory>


using namespace std;


WinRegKey::WinRegKey() :_key(nullptr), _access(KEY_ALL_ACCESS)
{


}

WinRegKey::~WinRegKey()
{
	if (_key != nullptr)
	{
		RegCloseKey(_key);
		_key = nullptr;
	}
}


bool WinRegKey::open_exist(HKEY root_key, LPCSTR sub_key, BOOL mode)
{
#if defined _WIN32 || defined (WIN32)
	//32bit program access 64bit regkey
	if (mode)
	{
		_access |= KEY_WOW64_64KEY;
	}
#elif definde (_WIN64) || defined(WIN64)
	//64bit program access 32bit regkey
	if (mode)
	{
		_access |= KEY_WOW64_32KEY;
	}
#endif
	_ret = RegOpenKeyExA(root_key, sub_key, 0, _access, &_key);
	return ERROR_SUCCESS == _ret; 
}


bool WinRegKey::open(HKEY root_key, LPCSTR sub_key, BOOL mode, BOOL read_only )
{
	DWORD dw = 0;
	if (read_only)
		_access = KEY_READ;
#if defined _WIN32 || defined (WIN32)
	//32bit program access 64bit regkey
	if (mode)
	{
		_access |= KEY_WOW64_64KEY;
	}
#elif definde (_WIN64) || defined(WIN64)
	//64bit program access 32bit regkey
	if (mode)
	{
		_access |= KEY_WOW64_32KEY;
	}
#endif
	_ret = RegCreateKeyExA(root_key,
		sub_key,
		0L,
		NULL,
		REG_OPTION_NON_VOLATILE, //the default. The information is stored in a file and is preserved when the system is restarted
		_access,
		NULL,
		&_key,
		&dw);
	return ERROR_SUCCESS == _ret;
}


bool WinRegKey::is_reg_path_exists(HKEY root_key, LPCSTR sub_key, BOOL mode)
{
	WinRegKey reg;
	return reg.open_exist(root_key, sub_key);
}

bool WinRegKey::read_key_value(LPCSTR key_name, DWORD& value) 
{
	DWORD type = 0;
	DWORD size = sizeof(DWORD);
	_ret = RegQueryValueExA(_key,
		key_name,
		NULL,
		&type,
		(BYTE*)&value,
		&size);
	return  ERROR_SUCCESS == _ret;
}

bool WinRegKey::read_key_value(LPCSTR key_name, char* const buf, int buf_len) 
{
	DWORD type = 0;
	DWORD size = buf_len;
	_ret = RegQueryValueExA(_key,
		key_name,
		NULL,
		&type,
		(BYTE*)buf,
		&size);
	return  ERROR_SUCCESS == _ret;

}

bool WinRegKey::read_key_value(LPCSTR key_name, string& content)
{
	DWORD size = 0;
	_ret = RegQueryValueExA(_key,
		key_name,
		NULL,
		nullptr,
		nullptr,
		&size);
	if (_ret != ERROR_SUCCESS)
		return false;
	string tmp = "";
	tmp.reserve(size);
	_ret = RegQueryValueExA(_key,
		key_name,
		NULL,
		nullptr,
		(LPBYTE)&tmp[0],
		&size);
	content.assign(tmp.data(), size);
	return  ERROR_SUCCESS == _ret;


}

bool WinRegKey::write_key_value(LPCSTR key_name, DWORD value) 
{
	 _ret = RegSetValueExA(_key,
		key_name,
		0,
		REG_DWORD,
		(const BYTE*)&value,
		sizeof(DWORD));
	return  ERROR_SUCCESS == _ret;
}

bool WinRegKey::write_key_value(LPCSTR key_name, const char* buf) 
{
	_ret = RegSetValueExA(_key,
		key_name,
		0,
		REG_SZ,
		(const BYTE*)buf,
		strlen(buf));
	return  ERROR_SUCCESS == _ret;
}

bool WinRegKey::write_key_value(LPCSTR key_name, string buf)
{
	_ret = RegSetValueExA(_key,
		key_name,
		0,
		REG_SZ,
		(const BYTE*)buf.c_str(),
		buf.size());
	return  ERROR_SUCCESS == _ret;
}

bool WinRegKey::write_key_encrypt_value(LPCSTR key_name, const char* buf)
{
	_ret = RegSetValueExA(_key,
		key_name,
		0,
		REG_BINARY,
		(const BYTE*)buf,
		strlen(buf));
	return  ERROR_SUCCESS == _ret;
}
bool WinRegKey::write_key_encrypt_value(LPCSTR key_name, string buf)
{
	_ret = RegSetValueExA(_key,
		key_name,
		0,
		REG_BINARY,
		(const BYTE*)buf.c_str(),
		buf.size());
	return  ERROR_SUCCESS == _ret;
}
bool WinRegKey::delete_key(LPCSTR key_name)
{
	typedef LONG(__stdcall *FUNCRegDeleteKeyExA)(HKEY hKey, LPCSTR lpSubKey, REGSAM samDesired, DWORD Reserved);

	HINSTANCE hLib = LoadLibraryA("Advapi32.dll");
	
	if (!hLib)
	{
		return false;
	}
	FUNCRegDeleteKeyExA pRegDeleteKey = (FUNCRegDeleteKeyExA)GetProcAddress(hLib, "RegDeleteKeyExA");
	//xp not support RegDeleteKeyEx
	if (!pRegDeleteKey)
	{
		_ret = RegDeleteKeyA(_key, key_name);
		
	}
	else
	{
		_ret = pRegDeleteKey(_key, key_name,_access, 0);
	}
	FreeLibrary(hLib);
	return  ERROR_SUCCESS == _ret;
}

/**/
string WinRegKey::get_error_msg()
{
	return WinOS::get_sys_error_msg(_ret);
}


vector<string>  WinRegKey::enum_subkeys()
{
	// The result subkey names will be stored here
	vector<string> subkeys;
	try {
		// Get some useful enumeration info, like the total number of subkeys
		// and the maximum length of the subkey names
		DWORD sub_key_count = 0;
		DWORD max_sub_key_name_len = 0;
		LONG ret = RegQueryInfoKeyA(
			_key,
			nullptr,    // no user-defined class
			nullptr,    // no user-defined class size
			nullptr,    // reserved
			&sub_key_count,
			&max_sub_key_name_len,
			nullptr,    // no subkey class length
			nullptr,    // no value count
			nullptr,    // no value name max length
			nullptr,    // no max value length
			nullptr,    // no security descriptor
			nullptr     // no last write time
		);
		WinRuntimeError::runtime_error_assert(ret == ERROR_SUCCESS, "Enum key failed %s",
			WinOS::get_sys_error_msg(ret));
		// NOTE: According to the MSDN documentation, the size returned for subkey name max length
		// does *not* include the terminating NUL, so let's add +1 to take it into account
		// when I allocate the buffer for reading subkey names.
		max_sub_key_name_len++;
		// Preallocate a buffer for the subkey names !!Note
		auto buf = make_unique<char[]>(max_sub_key_name_len);
		// Enumerate all the subkeys
		
		for (DWORD index = 0; index < sub_key_count; index++)
		{
			// Get the name of the current subkey
			DWORD len = max_sub_key_name_len;
			ret = RegEnumKeyExA(
				_key,
				index,
				buf.get(),
				&len,
				nullptr, // reserved
				nullptr, // no class
				nullptr, // no class
				nullptr  // no last write time
			);
			WinRuntimeError::runtime_error_assert(ret == ERROR_SUCCESS, "Enum key failed %s",
				WinOS::get_sys_error_msg(ret));
			// On success, the ::RegEnumKeyEx API writes the length of the
			// subkey name in the subKeyNameLen output parameter 
			// (not including the terminating NUL).
			// So I can build a wstring based on that length.
			subkeys.push_back(string{ buf.get(), len });
		}
		return subkeys;
	}
	catch (exception& e)
	{
		(void)e;
		return subkeys;
	}
}


