// SystemParametersInfo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")    
using namespace std;


void SaveOriginData()
{
	std::string strJson;
	DWORD dwAct;
	DWORD dwSec;
	DWORD dwTime;

	BOOL bAct;
	BOOL bSec;
	BOOL bRet;

	std::wstring wstrEXE;
	std::string strEXE;

	bRet = SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &bAct, 0);
	if (!bRet)
	{
		printf(("BaseLineScrnSave::RestoreOriginData Set 'SPI_SETSCREENSAVEACTIVE' Failed"));
	}

	bRet = SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &dwTime, 0);
	if (!bRet)
	{
		printf(("BaseLineScrnSave::RestoreOriginData Set 'SPI_SETSCREENSAVETIMEOUT' Failed"));
	}
	bRet = SystemParametersInfo(SPI_GETSCREENSAVESECURE, 0, &bSec, 0);
	if (!bRet)
	{
		printf(("BaseLineScrnSave::RestoreOriginData Set 'SPI_SETSCREENSAVESECURE' Failed"));
	}

	dwAct = bAct == TRUE ? 1 : 0; // 1 表示开启
	dwSec = bSec == TRUE ? 1 : 0; // 1 表示开启

	cout << "恢复时登录：" << dwSec << '\t' << dwTime << '\t' << dwAct << endl;

}

void main()
{
	SaveOriginData();
	BOOL fResult;
	int nTimeOut;
	BOOL bActive;
	DWORD dwTime = 0;
	DWORD dwRet;
	fResult = SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT,
		0,
		&dwTime,
		0);

	dwRet = (dwTime == 60) ? 1 : 0;

	cout << "当前系统息屏时间:" << dwTime << ':' << dwRet << endl;
	fResult = SystemParametersInfo(SPI_GETSCREENSAVEACTIVE,
		0,
		&bActive,
		0);

	// Double it. 
	cout << "系统屏保状态" << ((bActive == 1) ? "启用" : "未启用") << "\n请输入锁屏时间：";
	cin >> nTimeOut;
	cout << endl;

	bActive = bActive == 0 ? 1 : 1;

	if (fResult)
	{

		fResult = SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT,      // Set mouse information
			nTimeOut,
			0,        // Mouse information
			SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);  // Update Win.ini
		cout << ((fResult == 1) ? "设置成功" : "设置失败") << endl;
		fResult =  SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,
			bActive,
			0,
			SPIF_SENDCHANGE | SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
		cout << ((fResult == 1) ? "设置成功" : "设置失败") << endl;
		fResult = SystemParametersInfo(SPI_SETSCREENSAVESECURE,
			bActive,
			0,
			SPIF_SENDCHANGE | SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
		cout << ((fResult == 1) ? "设置成功" : "设置失败") << endl;
	}

	int i = 0;
	while (i < nTimeOut)
	{
		cout << i+1 << '/' << nTimeOut << endl;
		i++;
		Sleep(1000);
	}

	return;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
