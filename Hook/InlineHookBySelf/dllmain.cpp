#include "includes.h"
typedef void(*hkFun)(int);
hkFun g_OldFun;

void hkFun1(int num)
{
	num *= 100;
	g_OldFun(num);
}

DWORD WINAPI InitHack(LPVOID hModule)
{
	::MessageBoxA(NULL, "提示", "开始Hook", MB_OK);

#ifdef _DEBUG
	//创建控制台
	//AllocConsole();
	//auto consfile = freopen("conout$", "w", stdout);
#endif

	// Microsoft.Terminal.Control.dll+1B7F9 
	g_OldFun = (hkFun)Hook((BYTE*)0x00B08B40, (BYTE*)hkFun1, 9);

	::MessageBoxA(NULL, "提示", "Hook成功", MB_OK);


	while (!GetAsyncKeyState(VK_END))
	{
		Sleep(1);
	}

#ifdef _DEBUG
	//创建控制台
	//if(consfile)
	//{
	//	fclose(consfile);
	//}
	//FreeConsole();
#endif

	FreeLibraryAndExitThread((HMODULE)hModule, 0);
}

BOOL APIENTRY DllMain(	HMODULE hModule,
						DWORD ul_reason_for_call,
						LPVOID lpResvered)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		::MessageBoxA(NULL, "提示", "注入成功", MB_OK);

		DisableThreadLibraryCalls(hModule);
		HANDLE hThread = CreateThread(NULL, 0, InitHack, hModule, 0, NULL);
		if (hThread != INVALID_HANDLE_VALUE)
		{
			::MessageBoxA(NULL, "提示", "Create Thread Failed", MB_OK);
			CloseHandle(hThread);
		}
		else
		{
			::MessageBoxA(NULL, "提示", "结束", MB_OK);
		}
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		break;
	}
	default:
		break;
	}
}