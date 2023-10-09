#pragma once

#include <windows.h>
#include <vector>
#include <string>

#define HKCR	HKEY_CLASSES_ROOT
#define HKCU	HKEY_CURRENT_USER
#define HKLM	HKEY_LOCAL_MACHINE
#define HKU		HKEY_USERS
class WinRegKey
{

public:
	/** Check if the RegKey is existed
	*   mode : true		64bit program access 32bit regkey or  32bit program access 64bit regkey
	*/
	static bool is_reg_path_exists(HKEY root_key, LPCSTR sub_key, BOOL mode = FALSE);
public:
	/** Construtor
	*/
	WinRegKey();
	/**
	*/
	~WinRegKey();
	/** creat key if not exist. but also open exist key
	*/
	bool open_exist(HKEY root_key, LPCSTR sub_key, BOOL mode = FALSE);
	/**
	*/
	bool open(HKEY root_key, LPCSTR sub_key, BOOL mode = FALSE, BOOL read_only = FALSE);
	/** Read key value
	*/
	bool read_key_value(LPCSTR key_name, DWORD& value) ;
	bool read_key_value(LPCSTR key_name, char* const buf, int buf_len) ;
	bool read_key_value(LPCSTR key_name, std::string& content);
	/** Write key value
	*/
	bool write_key_value(LPCSTR key_name, DWORD value) ;
	bool write_key_value(LPCSTR key_name, const char* buf) ;
	bool write_key_value(LPCSTR key_name, std::string buf);
	/* may be only work for aes, to verify~
	*/
	bool write_key_encrypt_value(LPCSTR key_name, const char* buf);
	bool write_key_encrypt_value(LPCSTR key_name, std::string buf);
	/** Delete key
	*/
	bool delete_key(LPCSTR key_name);
	/** 
	*/
	std::vector<std::string> enum_subkeys();

	/**/
	std::string get_error_msg();
	/**/
	LONG get_error_code()const{ return _ret; }
protected:

private:
	HKEY						_key;
	REGSAM						_access;
	LONG						_ret;

};


