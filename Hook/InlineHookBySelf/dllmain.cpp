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
	::MessageBoxA(NULL, "��ʾ", "��ʼHook", MB_OK);

#ifdef _DEBUG
	//��������̨
	//AllocConsole();
	//auto consfile = freopen("conout$", "w", stdout);
#endif

	// Microsoft.Terminal.Control.dll+1B7F9 
	g_OldFun = (hkFun)Hook((BYTE*)0x00B08B40, (BYTE*)hkFun1, 9);

	::MessageBoxA(NULL, "��ʾ", "Hook�ɹ�", MB_OK);


	while (!GetAsyncKeyState(VK_END))
	{
		Sleep(1);
	}

#ifdef _DEBUG
	//��������̨
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
		::MessageBoxA(NULL, "��ʾ", "ע��ɹ�", MB_OK);

		DisableThreadLibraryCalls(hModule);
		HANDLE hThread = CreateThread(NULL, 0, InitHack, hModule, 0, NULL);
		if (hThread != INVALID_HANDLE_VALUE)
		{
			::MessageBoxA(NULL, "��ʾ", "Create Thread Failed", MB_OK);
			CloseHandle(hThread);
		}
		else
		{
			::MessageBoxA(NULL, "��ʾ", "����", MB_OK);
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