#include "CRegMonitorClass.h"

int main(int argc, const char* argv[])
{
	//Æô¶¯¼à¿ØÊ¾Àý
	CRegMonitorClass m_RegMon;

	m_RegMon.StartMonitor(HKEY_LOCAL_MACHINE, ("SYSTEM\\CurrentControlSet\\Services\\LanmanServer\\Shares"));

	system("pause");

	return 0;
}