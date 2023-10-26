/************************************************************************/

/* ShellDef: ͬʱ���һЩ��ͽṹ���� */

/************************************************************************/

#include <iostream>
#include <windows.h>
#include <shlobj_core.h>

#pragma comment(lib, "Shell32.lib")

#define SHCNRF_InterruptLevel 0x0001 //Interrupt level notifications from the file system

#define SHCNRF_ShellLevel 0x0002 //Shell-level notifications from the shell

#define SHCNRF_RecursiveInterrupt 0x1000 //Interrupt events on the whole subtree

#define SHCNRF_NewDelivery 0x8000 //Messages received use shared memorytypedef struct

#define WM_FILEMODIFY_NOTIFY 0x8888

void FileModifyNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

typedef LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


typedef struct _FILECHANGEINFO
{
	DWORD dwItem1; // dwItem1 contains the previous PIDL or name of the folder.

	DWORD dwItem2; // dwItem2 contains the new PIDL or name of the folder.

}FILECHANGEINFO;

typedef struct tagFILECHANGENOTIFY {

	DWORD dwRefCount;

	FILECHANGEINFO fci;

} FILECHANGENOTIFY;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case SHCNE_NETSHARE:
		{
			std::cout << "��ص������¼�" << std::endl;
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		//�Լ�������¼�
		case WM_FILEMODIFY_NOTIFY:
		{
			FileModifyNotify(hWnd, message, wParam, lParam);

			break;
		}
		default:
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

	}

	return 0;
}

/************************************************************************/

/* ���ڴ�����������ļ���¼�Ľ�� */

/************************************************************************/
void FileModifyNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char* strOriginal;
	FILECHANGEINFO* pChangeFile = (FILECHANGEINFO*)wParam;
	TCHAR szFileName1[MAX_PATH] = { 0 };

	switch (lParam)
	{
	case SHCNE_MKDIR:
	{
		SHGetPathFromIDList((LPCITEMIDLIST)(pChangeFile->dwItem1), szFileName1);

		break;
	}
	case SHCNE_RMDIR:
	{
		SHGetPathFromIDList((LPCITEMIDLIST)(pChangeFile->dwItem1), szFileName1);

		//dwResult = SHGetFileInfo((TCHAR*)(pChangeFile->dwItem1), 0, &shFileInfo1, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_DISPLAYNAME);

		break;
	}
	case SHCNE_CREATE: //�����ļ�
	{
		SHGetPathFromIDList((LPCITEMIDLIST)(pChangeFile->dwItem1), szFileName1);

		break;
	}
	default:

		break;

	}
}

int main(int argc, const char* argv[])
{
	ULONG m_ulNotifyId;

	//��������
	SHChangeNotifyEntry pshcne = { 0 };
	pshcne.pidl = NULL;
	pshcne.fRecursive = TRUE;

	//ע��WM_FILEMODIFY_NOTIFY���Լ��������Ϣ

	m_ulNotifyId = SHChangeNotifyRegister(hWnd, SHCNRF_ShellLevel, SHCNE_ALLEVENTS, WM_FILEMODIFY_NOTIFY, 1, &pshcne);


	// ȡ��ע��֪ͨ  
	SHChangeNotifyDeregister(m_ulNotifyId);
}