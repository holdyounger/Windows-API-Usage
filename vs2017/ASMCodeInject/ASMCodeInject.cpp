//// 解决 (LPVOID)¶m, 参考： https://www.programmerall.com/article/5483346938/
//// 使用： ASMCodeInject.exe -s 0x55,0x8B,0xEC,0x68,0x10,0x2A,0x4E,0x00,0xFF,0x14,0x24,0x8B,0xE5,0x5D,0xC3
//
//#include "windows.h" 
//#include "stdio.h" 
//
//#include <string>
//#include <vector>
//#include <iostream>
//using namespace std;
//
//#include <TlHelp32.h>
//
//typedef struct _THREAD_PARAM
//{
//	FARPROC pFunc[2];               // LoadLibraryA(), GetProcAddress()函数地址
//} THREAD_PARAM, *PTHREAD_PARAM;
//
////BYTE g_InjectionCode[] =
////{
////	// 关闭驱动
////	0x55, 0x8B, 0xEC, 0x68, 0x10, 0x2A, 0x4E, 0x00, 0xFF, 0x14, 0x24, 0x8B, 0xE5, 0x5D, 0xC3
////
////	// 退出
////	// 0x55, 0x8B, 0xEC, 0x68, 0x70, 0xFF, 0x5B, 0x00, 0xFF, 0x14, 0x24, 0x8B, 0xE5, 0x5D, 0xC3
////
////	//关闭驱动并退出
////	// 0x55, 0x8B, 0xEC, 0x68, 0x10, 0x2A, 0x4E, 0x00, 0xFF, 0x14, 0x24, 0x68, 0x70, 0xFF, 0x5B, 0x00, 0xFF, 0x14, 0x24, 0x8B, 0xE5, 0x5D, 0xC3
////
////};
//
//BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)   //设置权限
//{
//	TOKEN_PRIVILEGES tp;
//	HANDLE hToken;
//	LUID luid;
//
//	if (!OpenProcessToken(GetCurrentProcess(),
//		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
//		&hToken))
//	{
//		printf("OpenProcessToken error: %u\n", GetLastError());
//		return FALSE;
//	}
//
//	if (!LookupPrivilegeValue(NULL,           // lookup privilege on local system 
//		lpszPrivilege,  // privilege to lookup  
//		&luid))        // receives LUID of privilege 
//	{
//		printf("LookupPrivilegeValue error: %u\n", GetLastError());
//		return FALSE;
//	}
//
//	tp.PrivilegeCount = 1;
//	tp.Privileges[0].Luid = luid;
//	if (bEnablePrivilege)
//		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
//	else
//		tp.Privileges[0].Attributes = 0;
//
//	// Enable the privilege or disable all privileges. 
//	if (!AdjustTokenPrivileges(hToken,
//		FALSE,
//		&tp,
//		sizeof(TOKEN_PRIVILEGES),
//		(PTOKEN_PRIVILEGES)NULL,
//		(PDWORD)NULL))
//	{
//		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
//		return FALSE;
//	}
//
//	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
//	{
//		printf("The token does not have the specified privilege. \n");
//		return FALSE;
//	}
//
//	return TRUE;
//}
//
//BOOL InjectCode(HANDLE hProcess, BYTE *g_InjectionCode, int nSize)   //关键注入部分
//{
//	HMODULE         hMod = NULL;
//	THREAD_PARAM    param = { 0, };
//	HANDLE          hThread = NULL;
//	LPVOID          pRemoteBuf[2] = { 0, };
//
//	printf("---InjectCode()\n");
//	//for (int i = 0; i < shellcode_byte_len; i++)
//	//{
//	//	printf("%02X", shellcode_byte[i]);
//	//	printf("\n");
//	//}
//	//return 0;
//
//	hMod = GetModuleHandleA("kernel32.dll");
//
//	param.pFunc[0] = GetProcAddress(hMod, "LoadLibraryA");
//	param.pFunc[1] = GetProcAddress(hMod, "GetProcAddress");  //获取两个函数的地址并储存进入自定义的线程参数结构体中
//
//	// Allocation for THREAD_PARAM                            //在对应的进程当中分配内存来存储data
//	if (!(pRemoteBuf[0] = VirtualAllocEx(hProcess,                  // hProcess 
//		NULL,                      // lpAddress 
//		sizeof(THREAD_PARAM),      // dwSize 
//		MEM_COMMIT,                // flAllocationType 
//		PAGE_READWRITE)))         // flProtect 
//	{
//		printf("VirtualAllocEx1() fail : err_code = %d\n", GetLastError());
//		return FALSE;
//	}
//
//	if (!WriteProcessMemory(hProcess,                               // hProcess 
//		pRemoteBuf[0],                          // lpBaseAddress 
//		(LPVOID)&param,                         // lpBuffer 
//		sizeof(THREAD_PARAM),                   // nSize 
//		NULL))                                 // [out] lpNumberOfBytesWritten 
//	{
//		printf("WriteProcessMemory() fail xxx : err_code = %d\n", GetLastError());  //写入我们的代码要用的数据
//		return FALSE;
//	}
//
//	// Allocation for ThreadProc() 
//	if (!(pRemoteBuf[1] = VirtualAllocEx(hProcess,                  // hProcess 
//		NULL,                      // lpAddress 
//		nSize,   // dwSize 
//		MEM_COMMIT,                // flAllocationType 
//		PAGE_EXECUTE_READWRITE))) // flProtect 
//	{
//		printf("VirtualAllocEx2() fail : err_code = %d\n", GetLastError());  //为我们的注入代码分配空间
//		return FALSE;
//	}
//
//
//	printf("pRemoteBuf[1]= %p, g_InjectionCode=%p\n", pRemoteBuf[1], g_InjectionCode);
//
//	for (size_t i = 0; i < nSize; i++)
//	{
//		printf("%x ", g_InjectionCode[i]);
//	}
//
//	printf("\n ");
//
//
//	if (!WriteProcessMemory(hProcess,                               // hProcess 
//		pRemoteBuf[1],                          // lpBaseAddress 
//		(LPVOID)&g_InjectionCode,               // lpBuffer 
//		nSize,                // nSize 
//		NULL))                                 // [out] lpNumberOfBytesWritten 
//	{
//		printf("WriteProcessMemory() fail : err_code = %d\n", GetLastError());   //写入我们的shellcode
//		return FALSE;
//	}
//
//	if (!(hThread = CreateRemoteThread(hProcess,                    // hProcess    //这里创建远程线程执行我们的shellcode
//		NULL,                        // lpThreadAttributes 
//		0,                           // dwStackSize 
//		(LPTHREAD_START_ROUTINE)pRemoteBuf[1],
//		pRemoteBuf[0],               // lpParameter 
//		0,                           // dwCreationFlags 
//		NULL)))                     // lpThreadId 
//	{
//		printf("CreateRemoteThread() fail : err_code = %d\n", GetLastError());
//		return FALSE;
//	}
//
//	WaitForSingleObject(hThread, INFINITE);
//
//	CloseHandle(hThread);
//	CloseHandle(hProcess);
//
//	return TRUE;
//}
//
///*
//int main(int argc, char *argv[])
//{
//
//	printf("\n argc: %d \n", argc);
//	// printf("\n argv[0]: %s \n", argv[0]);
//	// printf("\n argv[1]: %s \n", argv[1]);
//
//	for (int i = 0; i < argc; i++)
//	{
//		printf("\n argv %d: %s \n", i, argv[i]);
//	}
//
//	DWORD dwPID = 0;
//
//	//if (argc != 2)
//	//{
//	//	printf("\n USAGE  : %s <pid>\n", argv[0]);
//	//	return 1;
//	//}
//
//	if (argc < 2)
//	{
//		printf("\n USAGE  : %s <pid>\n", argv[0]);
//		return 1;
//	}
//
//	// change privilege
//	if (!SetPrivilege(SE_DEBUG_NAME, TRUE))
//		return 1;
//
//	// code injection
//	dwPID = (DWORD)atol(argv[1]);
//	InjectCode(dwPID);
//	printf("Shellcode Successful injection.");
//	// printf("命中目标进程： 11672\n");
//	// printf("Shellcode 成功注入！");
//
//	return 0;
//}
//*/
//
//// 字符串替换 src_str：源字符串 old_str：待替换的字符串 new_str：替换的字符串
//// 参考： C++实现字符串替换的两种方法 https://blog.csdn.net/crazyer2010/article/details/9063847
//string str_replace(string&src_str, const string&old_str, const string&new_str)
//{
//	string::size_type pos = 0;
//	string::size_type a = old_str.size();
//	string::size_type b = new_str.size();
//	while ((pos = src_str.find(old_str, pos)) != string::npos)
//	{
//		src_str.replace(pos, a, new_str);
//		pos += b;
//	}
//	return src_str;
//}
//
//// 十六进制字符串转字节数组
//// 参考： Hex String to BYTE c++ https://stackoverflow.com/questions/59069111/hex-string-to-byte-c
//std::vector<BYTE> HexToBytes(const std::string& hex)
//{
//	std::vector<BYTE> bytes;
//	for (unsigned int i = 0; i < hex.length(); i += 2) {
//		std::string byteString = hex.substr(i, 2);
//		char byte = (char)strtol(byteString.c_str(), NULL, 16);
//		bytes.push_back(byte);
//	}
//
//	return bytes;
//}
//
///*
//int main(int argc, char *argv[])
//{
//
//	//DWORD dwPID = 0;
//	//dwPID = (DWORD)atol("37652");
//	//InjectCode(dwPID);
//
//	// 获取参数 参考：编写获取命令行参数Getopt函数(C++) https://blog.csdn.net/u011073673/article/details/73183849
//	int pid = -1; // 进程PID， 形如 11390
//	string shellcode_str = ""; // 注入进程的Shellcode，形如 0x55,0x8B,0xEC,0x68,0x10,0x2A,0x4E,0x00,0xFF,0x14,0x24,0x8B,0xE5,0x5D,0xC3
//
//	// "i:r:d:"
//	for (int i = 1; i < argc; i++)
//	{
//		if (!strcmp(argv[i], "-p")) // 获取整形数据
//		{
//			pid = atoi(argv[i + 1]);
//			printf("process_pid: %d \n", pid);
//		}
//
//		if (!strcmp(argv[i], "-s")) // 获取字符串或数组
//		{
//			shellcode_str = argv[i + 1];
//			printf("shellcode_str: %s \n", shellcode_str);
//		}
//
//	}
//
//	//替换Shellcode中的逗号和0x
//	shellcode_str = str_replace(shellcode_str, ",", "");
//	printf("shellcode_str: %s \n", shellcode_str);
//	shellcode_str = str_replace(shellcode_str, "0x", "");
//	printf("shellcode_str: %s \n", shellcode_str);
//
//	// Shellcode转换为字节数组
//	std::vector<BYTE> shellcode_byte = HexToBytes(shellcode_str);
//
//	int shellcode_byte_len = shellcode_byte.size();
//	printf("shellcode_byte_len: %d \n", shellcode_byte_len);
//	for (int i = 0; i < shellcode_byte_len; i++)
//	{
//		printf("%02X", shellcode_byte[i]);
//		printf("\n");
//	}
//
//	return 0;
//}
//*/
//
//int main(int argc, char *argv[])
//{
//
//	DWORD pid = 0; // 进程PID， 形如 11390
//	HANDLE hProcess = NULL; // 进程句柄
//	wstring processName = L"dscalltest.exe"; // 进程名
//
//	// 获取参数 参考：编写获取命令行参数Getopt函数(C++) https://blog.csdn.net/u011073673/article/details/73183849
//	string shellcode_str = ""; // 注入进程的Shellcode，形如 0x55,0x8B,0xEC,0x68,0x10,0x2A,0x4E,0x00,0xFF,0x14,0x24,0x8B,0xE5,0x5D,0xC3
//
//	// "i:r:d:"
//	//for (int i = 1; i < argc; i++)
//	//{
//	//	if (!strcmp(argv[i], "-s")) // 获取字符串或数组
//	//	{
//	//		shellcode_str = argv[i + 1];
//	//		printf("shellcode_str: %s \n", shellcode_str);
//	//	}
//	//}
//
//	shellcode_str = "0x55,0x8B,0xEC,0x68,0x10,0x2A,0x4E,0x00,0xFF,0x14,0x24,0x8B,0xE5,0x5D,0xC3";
//
//	if (shellcode_str == "")
//	{
//		printf("Shellcode 获取失败 : err_code = %d\n", GetLastError());
//		return 0;
//	}
//
//	//替换Shellcode中的逗号和0x
//	shellcode_str = str_replace(shellcode_str, ",", "");
//	printf("shellcode_str: %s \n", shellcode_str);
//	shellcode_str = str_replace(shellcode_str, "0x", "");
//	printf("shellcode_str: %s \n", shellcode_str);
//
//	// Shellcode转换为字节数组
//	//16进制字符串转换为字节数组
//	int shellcode_byte_len = shellcode_str.length() / 2;
//	printf("shellcode_str_len: %d \n", shellcode_str.length());
//	printf("shellcode_byte_len: %d \n", shellcode_byte_len);
//	//unsigned char* shellcode_byte = new unsigned char[shellcode_byte_len+1];
//
//	std::vector<BYTE> shellcode_byte = HexToBytes(shellcode_str);
//	//memcpy(shellcode_byte, byteRes, shellcode_str.length() / 2);
//	unsigned char* shellcode_arg = new unsigned char[shellcode_byte.size()];
//
//	for (int i = 0; i < shellcode_byte.size(); i++) {
//		shellcode_arg[i] = shellcode_byte[i];
//	}
//
//	if (sizeof(shellcode_byte) < 1) {
//		printf("Shellcode格式化失败，请检查输入参数");
//		return 0;
//	}
//	// ByteVector 转 BYTE
//	for (int i = 0; i < shellcode_byte.size(); i++)
//	{
//		printf("%02X\n", shellcode_byte[i]);
//	}
//
//	//while (shellcode_res != NULL) {
//	//	printf("---%02X\n",*shellcode_res);
//	//}
//
//	// 打印Shellcode
//	//int shellcode_byte_len = shellcode_byte.size();
//	//printf("shellcode_byte_len: %d \n", shellcode_byte_len);
//	//for (int i = 0; i < shellcode_byte_len; i++)
//	//{
//	//	printf("%02X", shellcode_byte[i]);
//	//	printf("\n");
//	//}
//
//
//	// 根据进程名获取进程ID
//	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  // 进程快照句柄
//	PROCESSENTRY32 process = { sizeof(PROCESSENTRY32) };	// 存放进程快照的结构体
//	// 遍历进程
//	while (Process32Next(hProcessSnap, &process)) {
//		// 找到 QQMusic.exe 进程
//		wstring s_szExeFile = process.szExeFile; // char* 转 string
//		if (s_szExeFile == processName) {
//			// 进程PID
//			pid = process.th32ProcessID;
//			if (pid < 1)
//			{
//				printf("OpenProcess() fail : err_code = %d\n", GetLastError());  //根据PID获取进程句柄
//				return 0;
//			}
//			printf("pid: %d \n", pid);
//			// 进程句柄
//			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process.th32ProcessID); // 进程句柄
//			if (!hProcess) {
//				printf("OpenProcess() get hProcess fail : err_code = %d\n", GetLastError());  //根据PID获取进程句柄
//				return 0;
//			}
//			// InjectCode(hProcess);
//			// 跳出循环
//			break;
//		}
//	}
//
//	InjectCode(hProcess, shellcode_arg, shellcode_byte.size());
//
//}
