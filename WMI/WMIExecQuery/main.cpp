#include "CWmiInfo.h"
#include <iostream>
using namespace std;

int main(int argc, const char* argv[])
{
	CWmiInfo wmiInfo;

	wmiInfo.InitWmi();

	{
		CString strRetValue;
		wmiInfo.GetSingleItemInfo(_T("Win32_Processor"), _T("Caption"), strRetValue);

		printf("'select Caption from Win32_Processor' %ws", strRetValue.GetBuffer());
	}

	{
		CString strRetValue;
		CString  strClassMem[] = { _T("Caption"),_T("CurrentClockSpeed"),_T("DeviceID"),_T("Manufacturer") };
		wmiInfo.GetGroupItemInfo(_T("Win32_Processor"), strClassMem, 4, strRetValue);

		printf("'select Caption,CurrentClockSpeed,DeviceID,Manufacturer from Win32_Processor'\n: %ws", strRetValue.GetBuffer
		());

	}


	wmiInfo.ReleaseWmi();

	return 0;
}